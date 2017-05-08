
#include "precompiled.h"
#include "VideoPlayer.h"
#include "Util.h"


VideoPlayer::VideoPlayer(QObject *parent): QThread(parent)
{
    m_FrameBuffer = new BufferQueue<cv::Mat>();
    m_VideoReader = new VideoReader(this, m_FrameBuffer);
    m_pMOG = cv::createBackgroundSubtractorMOG2();
    m_pMOG->setShadowValue(0);
    m_processor = new VideoProcess();
}
void VideoPlayer::run()
{
    int delay = (1000 / m_frameRate);
    while (!m_stop && !m_FrameBuffer->empty())
    {
        m_RGBframe = m_FrameBuffer->dequeue();
        if (m_RGBframe.empty())
        {
            msleep(delay);
            continue;
        }
        m_CurrentFrame++;
        m_processor->exec(m_RGBframe, m_RGBframe);
//        m_img = Util::toQt(m_RGBframe, QImage::Format_RGB888);
        m_img = Util::RGBMattoQt(m_RGBframe, QImage::Format_RGB888);
        emit playerFrame(m_img);
        msleep(delay);
    }
}

//template<typename ImgProcessor>
void VideoPlayer::processImage()
{
    // Background Subtruction MOG
    cv::blur(m_RGBframe, m_RGBframe, cv::Size(5, 5));
    m_pMOG->apply(m_RGBframe, m_fgMaskMOG2);
    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;
    // Default start
    Dilation(0, 0);
    Erosion(0, 0);
    cv::connectedComponentsWithStats(m_fgMaskMOG2, labels, stats, centroids);
//    process->setIMG(&m_RGBframe);
//    process->processImage();
//    m_frame_out = process->getIMG();
    m_img = Util::toQt(m_frame_out, QImage::Format_RGB888);
}

/**  @function Erosion  */
void VideoPlayer::Erosion(int, void *)
{
    cv::Mat element = cv::getStructuringElement(m_erosion_elem,
                                                cv::Size(2 * m_erosion_size + 1, 2 * m_erosion_size + 1),
                                                cv::Point(m_erosion_size, m_erosion_size));
    /// Apply the erosion operation
    cv::erode(m_fgMaskMOG2_dilated, m_fgMaskMOG2_noHole, element);
}

/** @function Dilation */
void VideoPlayer::Dilation(int, void *)
{
    cv::Mat element = cv::getStructuringElement(m_dilation_elem,
                                                cv::Size(2 * m_dilation_size + 1, 2 * m_dilation_size + 1),
                                                cv::Point(m_dilation_size, m_dilation_size));
    /// Apply the dilation operation
    cv::dilate(m_fgMaskMOG2, m_fgMaskMOG2_dilated, element);
}



void VideoPlayer::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, nullptr);
}




void VideoPlayer::initializeVideo()
{
    while (m_FrameBuffer->size() < 1) {};
    m_RGBframe = m_FrameBuffer->dequeue();
    m_CurrentFrame++;
//    m_img = Util::toQt(m_RGBframe, QImage::Format_RGB888);
    m_img = Util::RGBMattoQt(m_RGBframe, QImage::Format_RGB888);
}

bool VideoPlayer::loadVideo(std::string filename)
{
    bool isLoaded = m_VideoReader->loadVideo(filename);
    if (isLoaded)
    {
        m_frameRate = m_VideoReader->getFrameRate();
        m_VideoReader->startBuffer();
    }
    initializeVideo();
    emit playerFrame(m_img);
    return isLoaded;
}

void VideoPlayer::playVideo()
{
    if (!isRunning())
    {
        if (isStopped())
            m_stop = false;
        start(LowPriority);
    }
}

void VideoPlayer::setCurrentFrame(int frameNumber)
{
    m_VideoReader->stopBuffer();
    m_CurrentFrame = frameNumber;
    m_VideoReader->setCurrentFrame(m_CurrentFrame);
    m_VideoReader->startBuffer();
    initializeVideo();
    emit playerFrame(m_img);
}

double VideoPlayer::getCurrentFrame()
{
    return m_CurrentFrame;
}

std::tuple<int, int> VideoPlayer::getFrameSize()
{
    return std::make_tuple(m_VideoReader->getFrameWidth(), m_VideoReader->getFrameHeight());
}


VideoPlayer::~VideoPlayer()
{
    delete m_VideoReader;
    m_waitcond.wakeOne();
    wait();
}
