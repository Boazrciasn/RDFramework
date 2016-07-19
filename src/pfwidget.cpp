#include "pfwidget.h"
#include "ui_pfwidget.h"

PFWidget::PFWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PFWidget)
{
    myPlayer = new videoPlayer(this);
    QObject::connect(myPlayer, SIGNAL(processedImage(QImage)), SLOT(updatePlayerUI(QImage)));
    ui->setupUi(this);
    ui->start_pushButton->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
}

PFWidget::~PFWidget()
{
    delete ui;
}

void PFWidget::updatePlayerUI(QImage img)
{
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

void PFWidget::processFrame(QImage image)
{
    qDebug() << "Testing";
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

void PFWidget::onActionPlay()
{
    myPlayer->playVideo();
}

void PFWidget::onActionPause()
{
    myPlayer->stopVideo();
}

void PFWidget::onActionStop()
{
    myPlayer->stopVideo();
}


