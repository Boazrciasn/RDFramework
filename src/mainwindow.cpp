#include "include/mainwindow.h"
#include "include/annextractordialog.h"
#include "include/TextRegionDetector.h"
#include "ui_mainwindow.h"
#include <QDebug>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    reader = new Reader();
    ui->setupUi(this);
    ui->label->setScaledContents(true);
    ui->IMG_button->setChecked(true);
    ui->Rectangle_button->setChecked(true);
    connect(ui->runButton, SIGNAL (clicked()), this, SLOT (update()));
}

// ****************************** //
// ****************************** //

void MainWindow::update()
{
    if (ui->IMG_button->isChecked()){
        process_type = 0;
        //        processImg();
    }
    else if (ui->Video_button->isChecked()){
        process_type = 1;
        //        processVideo();
    }
    else
        process_type = -1;
}

// ****************************** //
// ****************************** //

MainWindow::~MainWindow()
{
    delete ui;
    //    delete PF;
}

void MainWindow::on_browse_clicked()
{
    dir = QFileDialog::getExistingDirectory(this,tr("Open Image Direrctory"), "/home/vvglab/Desktop/ImageCLEF2016/pages_devel"/*QDir::currentPath()*/,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    fileIndex = 0;
    reader->readFromTo(dir.toStdString(),this->fNames);

    if (!this->fNames.empty())
    {
        QString fileName = dir + "/" +this->fNames[fileIndex];
        QImage image(fileName + ".jpg");


        // Testing
        cv::Mat img_bw = Util::toCv(image,CV_8UC4);
        cv::cvtColor(img_bw, img_bw, CV_BGR2GRAY);
        cv::threshold(img_bw, img_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
        img_bw.convertTo(img_bw,CV_32FC1);
        img_bw = 255 - img_bw;

        QVector<QRect> out = TextRegionDetector::detectRegions(img_bw,this);

        QPainter qPainter(&image);
        qPainter.setBrush(Qt::NoBrush);
        qPainter.setPen(QPen(QColor(255, 0, 0), 2, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

        for (int i = 0; i < out.size(); i++) {
            qPainter.drawRect(out[i]);
        }
        image.save("out.jpg");


        QPixmap pixmap = QPixmap::fromImage(image);
        QImage scaledImage = pixmap.toImage().scaled(pixmap.size() * devicePixelRatio(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        scaledImage.setDevicePixelRatio(devicePixelRatio());
        QPixmap* newScaledPixmap = new QPixmap(QPixmap::fromImage(scaledImage));
        ui->label->setPixmap(*newScaledPixmap);

        ui->label->setScaledContents(true);
        ui->label->resize(ui->label->pixmap()->size());

//        mDialog = new MyDialog(this);
//        mDialog->show();
//        mDialog->setFNames(this->fNames,dir);
    }
}

// ****************************** //
// ****************************** //

void MainWindow::on_previous_clicked()
{
    fileIndex--;
    if (fileIndex < 0)
        fileIndex++;
    QString fileName = dir + "/" +this->fNames[fileIndex] + ".jpg";
    QImage image(fileName);
    QPixmap pixmap = QPixmap::fromImage(image);
    QImage scaledImage = pixmap.toImage().scaled(pixmap.size() * devicePixelRatio(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(devicePixelRatio());
    QPixmap* newScaledPixmap = new QPixmap(QPixmap::fromImage(scaledImage));
    ui->label->setPixmap(*newScaledPixmap);

    ui->label->setScaledContents(true);
    ui->label->resize(ui->label->pixmap()->size());

//    mDialog->setLabel(image);
}

// ****************************** //
// ****************************** //

void MainWindow::on_next_clicked()
{
    fileIndex++;
    if (fileIndex >= (int)this->fNames.size())
        fileIndex--;
    QString fileName = dir + "/" +this->fNames[fileIndex] + ".jpg";
    QImage image(fileName);
    QPixmap pixmap = QPixmap::fromImage(image);
    QImage scaledImage = pixmap.toImage().scaled(pixmap.size() * devicePixelRatio(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(devicePixelRatio());
    QPixmap* newScaledPixmap = new QPixmap(QPixmap::fromImage(scaledImage));
    ui->label->setPixmap(*newScaledPixmap);

    ui->label->setScaledContents(true);
    ui->label->resize(ui->label->pixmap()->size());

}

void MainWindow::on_extractWords_clicked()
{
    QString saveDir = QFileDialog::getExistingDirectory(this,tr("Open Image Direrctory"), QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    pageParser = new PageParser();

    for (int i = 0; i < (int)this->fNames.size(); i++) {
        QString fileName = dir + "/" +this->fNames[i];
        pageParser->readFromTo(fileName,this->words,this->coords);
        pageParser->cropPolygons(fileName, saveDir,this->words,this->coords);
        this->words.clear();
        this->coords.clear();
    }


    delete pageParser;
    pageParser = NULL;
}

void MainWindow::on_actionAnnotation_Extractor_triggered()
{
    AnnExtractorDialog *mDialog = new AnnExtractorDialog(this);
    mDialog->exec();
}
