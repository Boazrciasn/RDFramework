#include "precompiled.h"
#include "PFExecutor.h"

PFExecutor::PFExecutor(QObject *parent): QThread(parent)
{
    m_stop = true;
    mPF = NULL;
    m_current_frame = 0;
    m_video_reader = new VideoReader(parent);
}

void PFExecutor::run()
{
    int delay = (1000 / m_frameRate);
    while(!m_stop && m_current_frame != m_numOfFrames)
    {
        m_RGBframe = m_video_reader->getFrame();
        if (m_RGBframe.empty())
        {
            msleep(delay);
            continue;
        }
        m_current_frame++;
        if(mPF != NULL)
            processFrame();
        else
            m_img = Util::toQt(m_RGBframe, QImage::Format_RGB888);
        emit processedImage(m_img);
        msleep(delay);
    }
}

void PFExecutor::processFrame()
{
    mPF->setIMG(&m_RGBframe);
    mPF->run();
    m_frame_out = mPF->getIMG();
    m_img = Util::toQt(m_frame_out, QImage::Format_RGB888);
}

void PFExecutor::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}


bool PFExecutor::loadVideo(std::string filename)
{
    bool isLoaded = m_video_reader->loadVideo(filename);
    if(isLoaded)
    {
        m_frameRate = m_video_reader->getFrameRate();
        m_numOfFrames = m_video_reader->getNumberOfFrames();
        m_video_reader->loadBuffer();
    }
    while(m_video_reader->getBufferSize() < 1 ) {};
    m_RGBframe = m_video_reader->getFrame();
    m_current_frame++;
    m_img = Util::toQt(m_RGBframe, QImage::Format_RGB888);
    if(mPF != NULL)
        processFrame();
    emit processedImage(m_img);
    return isLoaded;
}

void PFExecutor::playVideo()
{
    if(!isRunning())
    {
        if(isStopped())
            m_stop = false;
        start(LowPriority);
    }
}

void PFExecutor::stopVideo()
{
    m_stop = true;
}

bool PFExecutor::isStopped() const
{
    return m_stop;
}

void PFExecutor::setCurrentFrame(int frameNumber)
{
    m_current_frame = frameNumber;
}

double PFExecutor::getFrameRate()
{
    return m_frameRate;
}

double PFExecutor::getCurrentFrame()
{
    return m_current_frame;
}

double PFExecutor::getNumberOfFrames()
{
    return m_numOfFrames;
}

double PFExecutor::getFrameHeight()
{
    return m_video_reader->getFrameHeight();
}

double PFExecutor::getFrameWidth()
{
    return m_video_reader->getFrameWidth();
}


PFExecutor::~PFExecutor()
{
    m_mutex.lock();
    delete m_video_reader;
    m_waitcond.wakeOne();
    m_mutex.unlock();
    wait();
}
