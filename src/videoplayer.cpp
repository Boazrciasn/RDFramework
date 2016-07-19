#include "precompiled.h"

#include "videoplayer.h"

void videoPlayer::run()
{
    int delay = (1000 / m_frameRate);
    while(!m_stop)
    {
        bool capSuccess =  m_capture->read(m_frame);
        if (!capSuccess)
        {
            m_stop = true;
            qDebug() << "NULL";
        }
        cv::cvtColor(m_frame, m_RGBframe, CV_BGR2RGB);
        m_img = Util::toQt(m_RGBframe,QImage::Format_RGB888);
        emit processedImage(m_img);
        msleep(delay);
    }
}

void videoPlayer::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}

videoPlayer::videoPlayer(QObject *parent) : QThread(parent)
{
    m_stop = true;
}

bool videoPlayer::loadVideo(std::string filename)
{
    m_capture = new cv::VideoCapture(filename);
    if(m_capture->isOpened())
    {
        m_frameRate = (int) m_capture->get(CV_CAP_PROP_FPS);
        qDebug() << "file is open!";
        return true;
    }
    else{
        qDebug() << "Unable to open video file!";
        return false;
    }
}

void videoPlayer::playVideo()
{
    if(!isRunning())
    {
        if(isStopped())
        {
            m_stop = false;
        }
        start(LowPriority);
    }
}

void videoPlayer::stopVideo()
{
    m_stop = true;
}

bool videoPlayer::isStopped() const
{
    return m_stop;
}

void videoPlayer::setCurrentFrame(int frameNumber)
{
    m_capture->set(CV_CAP_PROP_POS_FRAMES, frameNumber);
}

double videoPlayer::getFrameRate()
{
    return m_frameRate;
}

double videoPlayer::getCurrentFrame()
{
    return m_capture->get(CV_CAP_PROP_POS_FRAMES);
}

double videoPlayer::getNumberOfFrames()
{
    return m_capture->get(CV_CAP_PROP_FRAME_COUNT);
}

double videoPlayer::getFrameHeight()
{
    return m_capture->get(CV_CAP_PROP_FRAME_HEIGHT);
}

double videoPlayer::getFrameWidth()
{
    return m_capture->get(CV_CAP_PROP_FRAME_WIDTH);
}


videoPlayer::~videoPlayer()
{
    m_mutex.lock();
    m_stop = true;
    m_capture->release();
    delete m_capture;
    m_waitcond.wakeOne();
    m_mutex.unlock();
    wait();
}
