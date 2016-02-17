#include "include/mydialog.h"
#include "ui_mydialog.h"
#include <QDebug>

MyDialog::MyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyDialog)
{
    ui->setupUi(this);

}

MyDialog::~MyDialog()
{
    delete ui;
}


void MyDialog::setLabel(QImage &img)
{
    ui->label->setPixmap(QPixmap::fromImage(img));
}

void MyDialog::setFNames(std::vector<QString> &fNames, QString dir)
{
    this->fNames = fNames;
    this->fileIndex = 0;
    this->dir = dir;

    QString fileName = this->dir + "/" + this->fNames[this->fileIndex] + ".jpg";
    QImage image(fileName);
    ui->label->setPixmap(QPixmap::fromImage(image));
}

void MyDialog::on_pushButton_clicked()
{
    fileIndex--;
    if (fileIndex < 0)
        fileIndex++;
    QString fileName = this->dir + "/" +this->fNames[fileIndex] + ".jpg";
    QImage image(fileName);
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->comboBox->setCurrentIndex(0);
}

void MyDialog::on_pushButton_2_clicked()
{
    fileIndex++;
    if (fileIndex >= (int)this->fNames.size())
        fileIndex--;
    QString fileName = this->dir + "/" +this->fNames[fileIndex] + ".jpg";
    QImage image(fileName);
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->comboBox->setCurrentIndex(0);
}

void MyDialog::on_comboBox_activated(int index)
{
    QString fileName = this->dir + "/" +this->fNames[fileIndex] + ".jpg";
    QImage image(fileName);
    QImage small = image.scaled(image.width() - image.width()*0.25*index ,image.height() - image.height()*0.25*index, Qt::KeepAspectRatio);
    if (index == 4)
        small = image.scaled(image.width() - image.width()*0.90 ,image.height() - image.height()*0.90, Qt::KeepAspectRatio);

    ui->label->setPixmap(QPixmap::fromImage(small));
//    qDebug() << index;
}
