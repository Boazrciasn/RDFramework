#include "src/util/precompiled.h"

#include "MyDialog.h"
#include "ui_mydialog.h"

#include "Util.h"

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
    //    // Compute Histogram
    //    QCustomPlot * customPlot = ui->plot_widget;
    //    cv::Mat im_gray, img_bw, hist;
    //    im_gray = Util::toCv(image,CV_8UC4);
    //    cv::cvtColor(im_gray, im_gray, CV_BGR2GRAY);
    //    cv::threshold(im_gray, img_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    //    cv::reduce(img_bw,hist,1,CV_REDUCE_SUM,CV_32SC1);
    //    QVector<double> x(101), y(101); // initialize with entries 0..100
    //    for (int i=0; i<101; ++i)
    //    {
    //      x[i] = i/50.0 - 1; // x goes from -1 to 1
    //      y[i] = x[i]*x[i];  // let's plot a quadratic function
    //    }
    //    // create graph and assign data to it:
    //    customPlot->addGraph();
    //    customPlot->graph(0)->setData(x, y);
    //    // give the axes some labels:
    //    customPlot->xAxis->setLabel("x");
    //    customPlot->yAxis->setLabel("y");
    //    // set axes ranges, so we see all data:
    //    customPlot->xAxis->setRange(-1, 1);
    //    customPlot->yAxis->setRange(0, 1);
    ui->label->setPixmap(QPixmap::fromImage(image));
}

void MyDialog::on_pushButton_clicked()
{
    fileIndex--;

    if (fileIndex < 0)
        fileIndex++;

    QString fileName = this->dir + "/" + this->fNames[fileIndex] + ".jpg";
    QImage image(fileName);
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->comboBox->setCurrentIndex(0);
}

void MyDialog::on_pushButton_2_clicked()
{
    fileIndex++;

    if (fileIndex >= (int)this->fNames.size())
        fileIndex--;

    QString fileName = this->dir + "/" + this->fNames[fileIndex] + ".jpg";
    QImage image(fileName);
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->comboBox->setCurrentIndex(0);
}

void MyDialog::on_comboBox_activated(int index)
{
    QString fileName = this->dir + "/" + this->fNames[fileIndex] + ".jpg";
    QImage image(fileName);
    QImage small = image.scaled(image.width() - image.width() * 0.25 * index ,
                                image.height() - image.height() * 0.25 * index, Qt::KeepAspectRatio);

    if (index == 4)
        small = image.scaled(image.width() - image.width() * 0.90 ,
                             image.height() - image.height() * 0.90, Qt::KeepAspectRatio);

    ui->label->setPixmap(QPixmap::fromImage(small));
    //    qDebug() << index;
}
