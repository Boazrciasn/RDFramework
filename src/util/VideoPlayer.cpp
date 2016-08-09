#include "src/util/precompiled.h"
#include "VideoPlayer.h"



VideoPlayer::VideoPlayer(QObject *parent): QThread(parent)
{
    m_FrameBuffer = new BufferQueue();
    m_VideoReader = new VideoReader(this, m_FrameBuffer);
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
        if (m_PF != NULL)
            processPF();
        else
            m_img = Util::toQt(m_RGBframe, QImage::Format_RGB888);
        emit playerFrame(m_img);
        msleep(delay);
    }
}

void VideoPlayer::processPF()
{
    m_PF->setIMG(&m_RGBframe);
    m_PF->processImage();
    m_frame_out = m_PF->getIMG();
    m_img = Util::toQt(m_frame_out, QImage::Format_RGB888);
}

void VideoPlayer::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}




void VideoPlayer::initializeVideo()
{
    while (m_FrameBuffer->getSize() < 1) {};
    m_RGBframe = m_FrameBuffer->dequeue();
    m_CurrentFrame++;
    m_img = Util::toQt(m_RGBframe, QImage::Format_RGB888);
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
    m_mutex.lock();
    delete m_VideoReader;
    m_waitcond.wakeOne();
    m_mutex.unlock();
    wait();
}
