#include "VideoPlayerGui.h"
#include "ui_VideoPlayerGui.h"

VideoPlayerGui::VideoPlayerGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoPlayerGui)
{
    m_VideoPlayer = new VideoPlayer(this);
    QObject::connect(m_VideoPlayer, SIGNAL(playerFrame(QImage)), SLOT(updatePlayerUI(QImage)));
    ui->setupUi(this);
    ui->Play_pushButton->setEnabled(false);
    ui->Frame_horizontalSlider->setEnabled(false);
}

VideoPlayerGui::~VideoPlayerGui()
{
    delete ui;
}

void VideoPlayerGui::updatePlayerUI(QImage img)
{
    if (!img.isNull())
    {
        ui->Display_label->setAlignment(Qt::AlignCenter);
        ui->Display_label->setPixmap(
            QPixmap::fromImage(img).scaled(ui->Display_label->size(), Qt::KeepAspectRatio, Qt::FastTransformation)
        );
        ui->Frame_horizontalSlider->setValue(m_VideoPlayer->getCurrentFrame());
    }
}

void VideoPlayerGui::onHorizontalSliderPressed()
{
    m_isPlaying = !m_VideoPlayer->isStopped();
    m_VideoPlayer->stopVideo();
}

void VideoPlayerGui::onHorizontalSliderReleased()
{
    m_VideoPlayer->setCurrentFrame(m_currFrame);
    if (m_isPlaying)
        m_VideoPlayer->playVideo();
}

void VideoPlayerGui::onHorizontalSliderMoved(int position)
{
    m_currFrame = position;
}

void VideoPlayerGui::onActionBrowse()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a File", "", "Video File (*.*)");
    QFileInfo name = filename;
    if (!m_VideoPlayer->loadVideo(filename.toStdString()))
    {
        QMessageBox msgBox;
        msgBox.setText(" Selected video could not be opened! ");
        msgBox.exec();
    }
    else
    {
        setWindowTitle(name.fileName());
        ui->Play_pushButton->setEnabled(true);
        ui->Frame_horizontalSlider->setEnabled(true);
        ui->Frame_horizontalSlider->setMaximum(m_VideoPlayer->getNumberOfFrames());
        std::tuple<int, int> framesize = m_VideoPlayer->getFrameSize();
        int frameWidth;
        int frameHeight;
        std::tie(frameWidth, frameHeight) = framesize;
        m_VideoLodaded = true;
    }
}

void VideoPlayerGui::onActionPlay()
{
    if (m_VideoLodaded)
        m_VideoPlayer->playVideo();
}

void VideoPlayerGui::onActionPause()
{
    if (m_VideoLodaded)
        m_VideoPlayer->stopVideo();

}
