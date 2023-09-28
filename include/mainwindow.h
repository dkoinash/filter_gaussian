#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QGraphicsScene>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QFileDialog>
#include <QLabel>
#include <QFuture>
#include <QtConcurrent>
#include <string>
#include <fstream>

int readData();

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openImage_triggered();

    void on_saveImage_triggered();

    void on_closePrg_triggered();

    void on_pushButton_clicked();

    void on_radiusEdit_textChanged(const QString &arg);

    void on_pBradUp_clicked();

    void on_pBradDown_clicked();

    void on_info_triggered();

    void on_update_clicked();

private:
    Ui::MainWindow *ui;
    QImage sourseImage;
    QImage filterImage;
    QFuture<QImage> futureImage;
    bool activateFilter = false;
    bool filterDone = false;
    int radiusBlur = 2;

};
#endif // MAINWINDOW_H
