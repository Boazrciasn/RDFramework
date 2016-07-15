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
    mVideoWidg = new VideoFrameGrabber(this);
    mPlayer->setVideoOutput(mVideoWidg);

    connect(mVideoWidg, SIGNAL(frameAvailable(QImage)), this, SLOT(processFrame(QImage)));
//    layout->addWidget(mVideoWidg);
}

PFWidget::~PFWidget()
{
    delete ui;
}

void PFWidget::processFrame(QImage image)
{
    qDebug()<<"Testing";
}

void PFWidget::onActionOpen()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a File","","Video File (*.*)");
    onActionStop();
    mPlayer->setMedia(QUrl::fromLocalFile(filename));
    onActionPlay();
}

void PFWidget::onActionPlay()
{
    mPlayer->play();
}

void PFWidget::onActionPause()
{
    mPlayer->pause();
}

void PFWidget::onActionStop()
{
    mPlayer->stop();
}


