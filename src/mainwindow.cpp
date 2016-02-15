#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
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

//void MainWindow::setLable()
//{
//    QImage image(out.data, out.cols, out.rows, out.step, QImage::Format_Indexed8);
//    QPixmap tmp = QPixmap::fromImage(image);
//    ui->label->setPixmap(tmp);
//    ui->label->resize(ui->label->pixmap()->size());
//    repaint();
//}

// ****************************** //
// ****************************** //

MainWindow::~MainWindow()
{
    delete ui;
//    delete PF;
}

void MainWindow::on_pushButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
    dialog.setViewMode(QFileDialog::Detail);
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Images"), "/home/vvglab/Pictures/pages_devel", tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty())
    {
        QImage image(fileName);
//        QImage small = image.scaled(ui->label->width(),ui->label->height(), Qt::KeepAspectRatio);

        ui->label->setPixmap(QPixmap::fromImage(image));
        ui->label->resize(ui->label->pixmap()->size());
//        repaint();
    }
    qDebug() << fileName;
}
