#include "pfwidget.h"
#include "ui_pfwidget.h"

PFWidget::PFWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PFWidget)
{
    ui->setupUi(this);

    QWidget* videoWidget = ui->video_widget;
    QGridLayout* layout = new QGridLayout(this);
    videoWidget->setLayout(layout);

    mPlayer = new QMediaPlayer(this);
    mVideoWidg = new QVideoWidget(this);
    mPlayer->setVideoOutput(mVideoWidg);
    layout->addWidget(mVideoWidg);
}

PFWidget::~PFWidget()
{
    delete ui;
}

void PFWidget::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a File","","Video File (*.*)");
    on_actionStop_triggered();
    mPlayer->setMedia(QUrl::fromLocalFile(filename));
    on_actionPlay_triggered();
}

void PFWidget::on_actionPlay_triggered()
{
    mPlayer->play();
}

void PFWidget::on_actionPause_triggered()
{
    mPlayer->pause();
}

void PFWidget::on_actionStop_triggered()
{
    mPlayer->stop();
}


