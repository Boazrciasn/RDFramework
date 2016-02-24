#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    reader = new Reader();
    ui->setupUi(this);
    ui->IMG_button->setChecked(true);
    ui->Rectangle_button->setChecked(true);
    connect(ui->runButton, SIGNAL (clicked()), this, SLOT (update()));

    //    QLabel *imageLabel = new QLabel;
    //    imageLabel->setBackgroundRole(QPalette::Base);
    //    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //    imageLabel->setScaledContents(true);

    //    QScrollArea *scrollArea = new QScrollArea;
    //    scrollArea->setBackgroundRole(QPalette::Dark);
    //    scrollArea->setWidget(imageLabel);
    //    setCentralWidget(scrollArea);
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
    dir = QFileDialog::getExistingDirectory(this,tr("Open Image Direrctory"), QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    fileIndex = 0;
    reader->readFromTo(dir.toStdString(),this->fNames);

    if (!this->fNames.empty())
    {
        QString fileName = dir + "/" +this->fNames[fileIndex];
        QImage image(fileName + ".jpg");
        ui->label->setPixmap(QPixmap::fromImage(image));
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
    ui->label->setPixmap(QPixmap::fromImage(image));
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
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->label->resize(ui->label->pixmap()->size());
//    mDialog->setLabel(image);
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
