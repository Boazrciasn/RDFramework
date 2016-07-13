#include "pfwidget.h"
#include "ui_pfwidget.h"

PFWidget::PFWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PFWidget)
{
    ui->setupUi(this);

    mPlayer = new QMediaPlayer(this);
    mVideoWidg = new QVideoWidget(this);
    mPlayer->setVideoOutput(mVideoWidg);
}

PFWidget::~PFWidget()
{
    delete ui;
}

void PFWidget::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a File","","Video File (*.avi, *.mpg, *.mp4)");

}
