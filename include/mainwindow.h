#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define QGRAPHICSEFFECT false

#include <fstream>
#include <string>

#include <QFileDialog>
#include <QFuture>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPainter>
#include <QProgressDialog>
#include <QtConcurrent>
#include <QElapsedTimer>

int
readData();

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

private slots:
  void on_openImage_triggered();

  void on_saveImage_triggered();

  void on_closePrg_triggered();

  void on_pushButton_clicked();

  void on_radiusEdit_textChanged(const QString& arg);

  void on_pBradUp_clicked();

  void on_pBradDown_clicked();

  void on_info_triggered();

  void on_update_clicked();

#if QGRAPHICSEFFECT
#else
private:
  void updateGaussian(const int radius);
#endif
private:
  Ui::MainWindow*                  ui;
  QImage                           m_sourseImage;
  QImage                           m_filterImage;
  QFuture<QImage>                  futureImage;
  bool                             m_activateFilter = false;
  bool                             m_filterDone     = false;
  int                              m_radiusBlur     = 2;
  QProgressDialog                  m_progress;
  std::vector<std::vector<double>> m_gaussian;
  QElapsedTimer m_compTime;
};
#endif // MAINWINDOW_H
