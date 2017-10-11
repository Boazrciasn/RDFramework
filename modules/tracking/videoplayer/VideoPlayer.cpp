
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
        m_frame = m_FrameBuffer->dequeue();
        if (m_frame.empty())
        {
            msleep(delay);
            continue;
        }
        m_CurrentFrame++;
        cv::resize(m_frame,m_frame,cv::Size(m_frame_w,m_frame_h));
        m_frame = m_frame(cv::Range(240, m_frame_h), cv::Range::all());     // TODO:: Remove for general purpose
        m_processor->exec(m_frame, m_frame);
        m_img = Util::toQt(m_frame, QImage::Format_RGB888);
        emit playerFrame(m_img);
        msleep(delay);
    }
}

//template<typename ImgProcessor>
void VideoPlayer::processImage()
{
    // Background Subtruction MOG
    cv::blur(m_frame, m_frame, cv::Size(5, 5));
    m_pMOG->apply(m_frame, m_fgMaskMOG2);
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
    m_frame = m_FrameBuffer->dequeue();

    cv::resize(m_frame,m_frame,cv::Size(m_frame_w,m_frame_h));
    m_frame = m_frame(cv::Range(240, m_frame_h), cv::Range::all());     // TODO:: Remove for general purpose

    m_CurrentFrame++;
    m_img = Util::toQt(m_frame, QImage::Format_RGB888);
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

double VideoPlayer::getCurrentFrameInd()
{
    return m_CurrentFrame;
}

std::tuple<int, int> VideoPlayer::getFrameSize()
{
    // TODO: fix it
    return std::make_tuple(m_frame_w, m_frame_h - 240);
//    return std::make_tuple(m_VideoReader->getFrameWidth(), m_VideoReader->getFrameHeight());
}


VideoPlayer::~VideoPlayer()
{
    delete m_VideoReader;
    m_waitcond.wakeOne();
    wait();
}
