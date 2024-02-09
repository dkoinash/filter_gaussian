#include "mainwindow.h"

#include <cmath>
#include <future>
#include <omp.h> // OpenMP
#include <vector>

#include <QTime>
#include <queue>

#include "./ui_mainwindow.h"
#include "about.h"

#define SIMPLE_G true

#define PI 3.1415926535

#if QGRAPHICSEFFECT
QImage
blurImage(const QImage& source, int blurRadius)
{
  if (source.isNull()) QImage();
  QGraphicsScene      scene;
  QGraphicsPixmapItem item;
  item.setPixmap(QPixmap::fromImage(source));
  auto* blur = new QGraphicsBlurEffect;
  if (blurRadius < 0)
    blurRadius = 0;
  else if (blurRadius > 20)
    blurRadius = 20;
  blur->setBlurRadius(blurRadius);
  item.setGraphicsEffect(blur);
  scene.addItem(&item);
  QImage result(source.size(), QImage::Format_ARGB32);
  result.fill(Qt::transparent);
  QPainter painter(&result);
  scene.render(&painter, QRectF(), QRect(0, 0, source.width(), source.height()));
  return result;
}
#else

QRgb
makePixel(const QImage&                            sourceImg,
          const std::vector<std::vector<double> >& gaussian,
          const int                                pos_x,
          const int                                pos_y)
{
  int gaussianSize = gaussian.size();
  int radius       = (gaussianSize + 1) / 2;

  double summRed   = 0;
  double summGreen = 0;
  double summBlue  = 0;

  {
    for (int row = 0; row < gaussianSize; ++row) {
      for (int col = 0; col < gaussianSize; ++col) {
        int img_x = pos_x - radius + 1 + col;
        int img_y = pos_y - radius + 1 + row;

        if (img_x < 0 || img_x >= sourceImg.width() || img_y < 0 || img_y >= sourceImg.height())
          continue;
        QRgb pixel = sourceImg.pixel(img_x, img_y);

        int redValue   = qRed(pixel);
        int greenValue = qGreen(pixel);
        int blueValue  = qBlue(pixel);

        double gaussianKoef = gaussian[col][row];

        summRed += redValue * gaussianKoef;
        summGreen += greenValue * gaussianKoef;
        summBlue += blueValue * gaussianKoef;
      }
    }
  }
  return qRgb(summRed, summGreen, summBlue);
}

QImage
blurImage(const QImage& sourseImage, const std::vector<std::vector<double> > gaussian)
{
  QImage result(sourseImage.size(), QImage::Format_ARGB32);
#ifndef QUELLE
#pragma omp parallel
  {
    for (int row = 0; row < sourseImage.height(); ++row) {
#pragma omp for
      for (int col = 0; col < sourseImage.width(); ++col) {
        result.setPixelColor(col, row, makePixel(sourseImage, gaussian, col, row));
      }
    }
  }
#else
  std::queue<std::future<QRgb> > listColor;
  for (int row = 0; row < sourseImage.height(); ++row) {
    for (int col = 0; col < sourseImage.width(); ++col) {
      listColor.push(std::async(std::launch::async, makePixel, sourseImage, gaussian, col, row));
    }
  }

  for (int row = 0; row < sourseImage.height(); ++row) {
    for (int col = 0; col < sourseImage.width(); ++col) {
      listColor.front().wait();
      result.setPixelColor(col, row, listColor.front().get());
      listColor.pop();
    }
  }

#endif

  return result;
}
#endif

void
saveData(int radius)
{
  std::fstream data;
  data.open("data.txt", std::fstream::out);
  if (data.is_open()) {
    std::string msg = std::to_string(radius);
    data << msg;
  }
  data.close();
}
int
readData()
{
  std::fstream data;
  data.open("data.txt", std::fstream::in);
  if (data.is_open()) {
    std::string msg;
    data >> msg;
    return std::stoi(msg);
  }
  data.close();
  return 2;
}

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->radiusEdit->setText(QString::number(m_radiusBlur));
  m_progress.setRange(0, 0);
  m_progress.setCancelButton(0);
  m_progress.setWindowFlags(Qt::WindowStaysOnTopHint /*| Qt::Window | Qt::WindowTitleHint |
                            Qt::CustomizeWindowHint*/);
  m_progress.close();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void
MainWindow::on_openImage_triggered()
{
  ui->statusbar->showMessage("Открыть изображение");
  m_radiusBlur = readData();
  ui->radiusEdit->setText(QString::number(m_radiusBlur));

  QFuture<int> futureLoad = QtConcurrent::run(readData);
  futureLoad.waitForFinished();

  m_radiusBlur = futureLoad.result();
  ui->radiusEdit->setText(QString::number(m_radiusBlur));
  QString filePath = QFileDialog::getOpenFileName(
    nullptr, "Open image file..", ".\\", "(*.jpg *.jpeg *.bmp, *.png)");
  ui->statusbar->showMessage(filePath);
  m_sourseImage = QImage(filePath);
  ui->soursePicLabel->setPixmap(QPixmap::fromImage(m_sourseImage));
}

void
MainWindow::on_saveImage_triggered()
{
  ui->statusbar->showMessage("Сохранить изображение");
  if (m_filterDone) {
    QString targetPath = QFileDialog::getSaveFileName(
      nullptr, "Save filter image..", ".\\", "(*.jpg *.jpeg, *.bmp, *.png)");
    m_filterImage.save(targetPath);
  }
}

void
MainWindow::on_closePrg_triggered()
{
  QFuture<void> futureSave = QtConcurrent::run(saveData, m_radiusBlur);
  futureSave.waitForFinished();
  QApplication::quit();
}

void
MainWindow::on_info_triggered()
{
  About window;
  window.setModal(true);
  window.exec();
}

void
MainWindow::on_pushButton_clicked()
{
  ui->statusbar->showMessage("Фильтр активирован.");
  m_activateFilter = true;
  m_filterDone     = false;

#if QGRAPHICSEFFECT
  futureImage = QtConcurrent::run(blurImage, m_sourseImage, m_radiusBlur);
#else
  futureImage = QtConcurrent::run(blurImage, m_sourseImage, m_gaussian);
#endif

  QFutureWatcher<QImage>* watcher = new QFutureWatcher<QImage>(this);
  connect(watcher, SIGNAL(finished()), this, SLOT(on_update_clicked()));
  connect(watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()));
  watcher->setFuture(futureImage);
  m_progress.open();
  m_compTime.restart();
  // m_compTime.start();
}

void
MainWindow::on_radiusEdit_textChanged(const QString& arg)
{
  m_radiusBlur = arg.toInt();
  if (m_radiusBlur > 20)
    m_radiusBlur = 20;
  else if (m_radiusBlur < 1)
    m_radiusBlur = 1;
#if QGRAPHICSEFFECT
    /// *
#else
  updateGaussian(m_radiusBlur);
#endif
}

void
MainWindow::on_pBradUp_clicked()
{
  ui->statusbar->showMessage("+1 к радиусу.");
  if (m_radiusBlur < 20) {
    m_radiusBlur += 1;
    ui->radiusEdit->setText(QString::number(m_radiusBlur));
#if QGRAPHICSEFFECT
    /// *
#else
    updateGaussian(m_radiusBlur);
#endif
  }
}

void
MainWindow::on_pBradDown_clicked()
{
  ui->statusbar->showMessage("-1 к радиусу.");
  if (m_radiusBlur > 2) {
    m_radiusBlur -= 1;
    ui->radiusEdit->setText(QString::number(m_radiusBlur));
#if QGRAPHICSEFFECT
    /// *
#else
    updateGaussian(m_radiusBlur);
#endif
  }
}

void
MainWindow::on_update_clicked()
{
  // ui->statusbar->showMessage("Обновить изображение.");
  m_progress.close();
  if (futureImage.isFinished() && m_activateFilter) {
    m_filterImage = futureImage.result();
    ui->filterPicLabel->setPixmap(
      QPixmap::fromImage(m_filterImage)); //.scaled(600,800, Qt::KeepAspectRatio));
    m_filterDone     = true;
    m_activateFilter = false;
  }
  int   duration = m_compTime.elapsed();
  QTime durationTime(0, 0, 0);

  ui->statusbar->showMessage("Время выполения преобразования: " +
                             durationTime.addMSecs(duration).toString("HH:mm:ss:zzz") + " мс.");
}
#if QGraphicsEffect
#else
void
MainWindow::updateGaussian(const int radius)
{
  m_gaussian.clear();
#if SIMPLE_G
  const int tableSize = radius * 2 + 1;

  const double sigma = (radius * 2 - 1) / std::sqrt(8 * std::log(2));

  for (int row = 0; row < tableSize; ++row) {
    std::vector<double> rowVec;
    rowVec.assign(tableSize, 0.0);
    m_gaussian.push_back(rowVec);
  }
  double summ = 0.0;
  for (int i = -radius; i <= radius; ++i) {
    for (int j = -radius; j <= radius; ++j) {
      double p                           = std::sqrt((i * i) + (j * j));
      double q                           = 2.0 * sigma * sigma;
      m_gaussian[j + radius][i + radius] = (std::exp(-(p * p) / q)) / (PI * q);
      summ += m_gaussian[j + radius][i + radius];
    }
  }
#else
  int tableSize = radius * 2 + 1;

  const double sigma = (radius * 2 + 1) / 2 * PI;

  for (int row = 0; row < tableSize; ++row) {
    std::vector<double> rowVec;
    rowVec.assign(tableSize, 0.0);
    m_gaussian.push_back(rowVec);
  }

  double summ = 0.0;
  for (int i = -radius; i <= radius; ++i) {
    for (int j = -radius; j <= radius; ++j) {
      m_gaussian[j + radius][i + radius] =
        std::pow(2, (radius - std::abs(i)) + (radius - std::abs(j)));
      summ += m_gaussian[j + radius][i + radius];
    }
  }
#endif
  for (int i = 0; i < tableSize; ++i) {
    for (int j = 0; j < tableSize; ++j) {
      m_gaussian[j][i] = m_gaussian[j][i] / summ;
    }
  }
  m_gaussian.shrink_to_fit();
}
#endif
