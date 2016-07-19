#include "pfwidget.h"
#include "ui_pfwidget.h"

#include "opencv2/opencv.hpp"
#include "SimplePF.h"
#include "Util.h"

PFWidget::PFWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PFWidget)
{
    ui->setupUi(this);

    mFrameLabel = ui->videoFrameLabel;





//    QWidget* videoWidget = ui->video_widget;
//    QGridLayout* layout = new QGridLayout(this);
//    videoWidget->setLayout(layout);

//    mPlayer = new QMediaPlayer(this);
//    mVideoWidg = new VideoFrameGrabber(this);
//    mPlayer->setVideoOutput(mVideoWidg);

//    connect(mVideoWidg, SIGNAL(frameAvailable(QImage)), this, SLOT(processFrame(QImage)));
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
    mVideoFile = QFileDialog::getOpenFileName(this, "Open a File","","Video File (*.*)");

//    onActionStop();
//    mPlayer->setMedia(QUrl::fromLocalFile(filename));
//    onActionPlay();
}

void PFWidget::onActionPlay()
{
//    mPlayer->play();

    cv::VideoCapture capture(mVideoFile.toStdString());

    if (!capture.isOpened()){
        qDebug() << "Unable to open video file: ";
        return;
    }

    cv::Mat frame;
    cv::Mat gray_frame;
    cv::Mat out;
    int nParticles = 100;
    int nIters = 10;

    SimplePF *PF = new SimplePF(&frame, nParticles, nIters);
    PF->setParticleWidth(55);

    while (true){
            if (capture.read(frame)){
                cv::cvtColor(frame, gray_frame, CV_BGR2GRAY);
                PF->setIMG(&gray_frame);
                PF->run();
                out = PF->getIMG();
                QImage img = Util::toQt(out,QImage::Format_RGB888);
                mFrameLabel->setPixmap(QPixmap::fromImage(img));
                mFrameLabel->repaint();
            }

            int keyboard = cvWaitKey(30); // wait 10 ms or for key stroke
            if (keyboard == 27)
                break; // if ESC, break and quit
        }
}

void PFWidget::onActionPause()
{
//    mPlayer->pause();
}

void PFWidget::onActionStop()
{
//    mPlayer->stop();
}


