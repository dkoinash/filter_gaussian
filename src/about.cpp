#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->label_1->setText("Фильтр");
    ui->label_2->setText("Обозначение");
    ui->label_3->setText("0.1a");
    ui->label_4->setText("Разработчик");
}

About::~About()
{
    delete ui;
}
