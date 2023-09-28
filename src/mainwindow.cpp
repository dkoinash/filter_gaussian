#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "about.h"

QImage blurImage(QImage source, int blurRadius)
{
    if(source.isNull())  QImage();
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(source));
    //
    auto *blur = new QGraphicsBlurEffect;

    if(blurRadius < 0 ) blurRadius = 0;
    else if(blurRadius > 10 ) blurRadius = 10;

    blur->setBlurRadius(blurRadius);
    item.setGraphicsEffect(blur);
    scene.addItem(&item);
    QImage result (source.size(), QImage::Format_ARGB32);
    result.fill(Qt::transparent);
    QPainter painter(&result);
    scene.render(&painter, QRectF(), QRect(0, 0, source.width(), source.height() ));
    return result;
}

void saveData(int radius)
{
    std::fstream data;
    data.open("data.txt", std::fstream::out);
    if(data.is_open()){
        std::string msg = std::to_string(radius);
        data << msg;
    }
    data.close();
}
int readData()
{
    std::fstream data;
    data.open("data.txt", std::fstream::in);
    if(data.is_open()){
        std::string msg;
        data >> msg;
        return std::stoi(msg);
    }
    data.close();
    return 0;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_openImage_triggered()
{
    ui->statusbar->showMessage("Открыть изображение");
    radiusBlur = readData();
    ui->radiusEdit->setText(QString::number(radiusBlur));
    QFuture<int> futureLoad = QtConcurrent::run(readData);
    futureLoad.waitForFinished();
    radiusBlur = futureLoad.result();
    ui->radiusEdit->setText(QString::number(radiusBlur));
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Open image file..", ".\\", "(*.jpg *.jpeg *.bmp, *.png)");
    ui->statusbar->showMessage(filePath);
    sourseImage = QImage(filePath);
    ui->soursePicLabel->setPixmap(QPixmap::fromImage(sourseImage));//.scaled(600,800, Qt::KeepAspectRatio));
}


void MainWindow::on_saveImage_triggered()
{
    ui->statusbar->showMessage("Сохранить изображение");
    if(filterDone){
        QString targetPath = QFileDialog::getSaveFileName(nullptr, "Save filter image..", ".\\", "(*.jpg *.jpeg, *.bmp, *.png)");
        filterImage.save(targetPath);
    }
}


void MainWindow::on_closePrg_triggered()
{
    QFuture<void> futureSave = QtConcurrent::run(saveData, radiusBlur);
    futureSave.waitForFinished();
    QApplication::quit();
}

void MainWindow::on_info_triggered()
{
    About window;
    window.setModal(true);
    window.exec();
}

void MainWindow::on_pushButton_clicked()
{
    ui->statusbar->showMessage("Фильтр активирован.");
    activateFilter = true;
    filterDone = false;
    futureImage = QtConcurrent::run(blurImage, sourseImage, radiusBlur);
}


void MainWindow::on_radiusEdit_textChanged(const QString &arg)
{
    radiusBlur = arg.toInt();
    if(radiusBlur > 20) radiusBlur = 20;
    else if(radiusBlur < 2) radiusBlur = 2;
}


void MainWindow::on_pBradUp_clicked()
{
    ui->statusbar->showMessage("+1 к радиусу.");
    if(radiusBlur < 20){
        radiusBlur += 1;
        ui->radiusEdit->setText(QString::number(radiusBlur));
    }
}


void MainWindow::on_pBradDown_clicked()
{
    ui->statusbar->showMessage("-1 к радиусу.");
    if(radiusBlur > 2){
        radiusBlur -= 1;
        ui->radiusEdit->setText(QString::number(radiusBlur));
    }
}


void MainWindow::on_update_clicked()
{
    ui->statusbar->showMessage("Обновить изображение.");
    if(futureImage.isFinished() && activateFilter){
        filterImage = futureImage.result();
        ui->filterPicLabel->setPixmap(QPixmap::fromImage(filterImage));//.scaled(600,800, Qt::KeepAspectRatio));
        filterDone = true;
        activateFilter = false;
    }
}





