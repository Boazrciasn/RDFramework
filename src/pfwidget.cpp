#include "pfwidget.h"
#include "ui_pfwidget.h"

PFWidget::PFWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PFWidget)
{
    myPlayer = new VideoPlayer(this);
    QObject::connect(myPlayer, SIGNAL(processedImage(QImage)), SLOT(updatePlayerUI(QImage)));
    ui->setupUi(this);
    ui->start_pushButton->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);

    nParticles = ui->particleCountSpinBox->value();
    nIters = ui->itteCountSpinBox->value();
    mParticleWidth = ui->particleWidthLSpinBox->value();

    mPF = new SimplePF(&mFrame, nParticles, nIters);
    mPF->setParticleWidth(mParticleWidth);
    myPlayer->setPF(mPF);
}

PFWidget::~PFWidget()
{
    delete ui;
}

void PFWidget::updatePlayerUI(QImage img)
{
//    mFrame = Util::toCv(frameImg,CV_8UC3);
//    cv::cvtColor(mFrame, mFrameGray, CV_BGR2GRAY);
//    mPF->setIMG(&mFrameGray);
//    mPF->run();
//    mFrameOut = mPF->getIMG();
//    QImage img = Util::toQt(mFrameOut,QImage::Format_RGB888);

    if (!img.isNull())
    {
        ui->display_label->setAlignment(Qt::AlignCenter);
        ui->display_label->setPixmap(QPixmap::fromImage(img).scaled(ui->display_label->size(), Qt::KeepAspectRatio,
                                                                    Qt::FastTransformation));
        ui->horizontalSlider->setValue(myPlayer->getCurrentFrame());
    }
}

QString PFWidget::getFormattedTime(int timeInSeconds)
{
    int seconds = (int) (timeInSeconds) % 60 ;
    int minutes = (int) ((timeInSeconds / 60) % 60);
    int hours   = (int) ((timeInSeconds / (60 * 60)) % 24);
    QTime t(hours, minutes, seconds);
    if (hours == 0 )
        return t.toString("mm:ss");
    else
        return t.toString("h:mm:ss");
}

void PFWidget::onHorizontalSliderPressed()
{
    myPlayer->stopVideo();
}

void PFWidget::onHorizontalSliderReleased()
{
    myPlayer->playVideo();
}

void PFWidget::onHorizontalSliderMoved(int position)
{
    myPlayer->setCurrentFrame(position);
}

void PFWidget::onActionOpen()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a File", "", "Video File (*.*)");
    QFileInfo name = filename;
    if (!myPlayer->loadVideo(filename.toStdString()))
    {
        QMessageBox msgBox;
        msgBox.setText(" Selected video could not be opened! ");
        msgBox.exec();
    }
    else
    {
        setWindowTitle(name.fileName());
        ui->start_pushButton->setEnabled(true);
        ui->horizontalSlider->setEnabled(true);
        ui->horizontalSlider->setMaximum(myPlayer->getNumberOfFrames());
        qDebug() << myPlayer->getNumberOfFrames();
        qDebug() <<  myPlayer->getFrameHeight();
        qDebug() << myPlayer->getFrameWidth();
    }
}

void PFWidget::onParticleCountChange(int value)
{
    nParticles = value;
    mPF->setNumParticles(nParticles);
}

void PFWidget::onIterationCountChange(int value)
{
    nIters = value;
    mPF->setNumIters(nIters);
}

void PFWidget::onParticleWidthChange(int value)
{
    mParticleWidth = value;
    mPF->setParticleWidth(mParticleWidth);
}


void PFWidget::onActionPlay()
{
    myPlayer->playVideo();
    ui->groupBox->setEnabled(false);
}

void PFWidget::onActionPause()
{
    myPlayer->stopVideo();
    ui->groupBox->setEnabled(true);
}

void PFWidget::onActionStop()
{
    myPlayer->stopVideo();
    ui->groupBox->setEnabled(true);
}
