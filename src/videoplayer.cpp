#include "precompiled.h"

#include "videoplayer.h"

void VideoPlayer::run()
{
    int delay = (1000 / m_frameRate);
    while(!m_stop)
    {
        bool capSuccess = m_capture->read(m_frame);
        if (!capSuccess)
        {
            m_stop = true;
            qDebug() << "NULL";
        }
        cv::cvtColor(m_frame, m_RGBframe, CV_BGR2RGB);
        m_img = Util::toQt(m_RGBframe,QImage::Format_RGB888);

        if(mPF != NULL)
            processFrame();
        emit processedImage(m_img);
        msleep(delay);
    }
}

void VideoPlayer::processFrame()
{
    cv::cvtColor(m_RGBframe, m_frame_gray, CV_BGR2GRAY);
    mPF->setIMG(&m_frame_gray);
    mPF->run();
    m_frame_out = mPF->getIMG();
    m_img = Util::toQt(m_frame_out,QImage::Format_RGB888);
}

void VideoPlayer::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}

VideoPlayer::VideoPlayer(QObject *parent) : QThread(parent)
{
    m_stop = true;
    mPF = NULL;
}

bool VideoPlayer::loadVideo(std::string filename)
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

void VideoPlayer::playVideo()
{
    if(!isRunning())
    {
        if(isStopped())
            m_stop = false;

        start(LowPriority);
    }
}

void VideoPlayer::stopVideo()
{
    m_stop = true;
}

bool VideoPlayer::isStopped() const
{
    return m_stop;
}

void VideoPlayer::setCurrentFrame(int frameNumber)
{
    m_capture->set(CV_CAP_PROP_POS_FRAMES, frameNumber);
}

double VideoPlayer::getFrameRate()
{
    return m_frameRate;
}

double VideoPlayer::getCurrentFrame()
{
    return m_capture->get(CV_CAP_PROP_POS_FRAMES);
}

double VideoPlayer::getNumberOfFrames()
{
    return m_capture->get(CV_CAP_PROP_FRAME_COUNT);
}

double VideoPlayer::getFrameHeight()
{
    return m_capture->get(CV_CAP_PROP_FRAME_HEIGHT);
}

double VideoPlayer::getFrameWidth()
{
    return m_capture->get(CV_CAP_PROP_FRAME_WIDTH);
}


VideoPlayer::~VideoPlayer()
{
    m_mutex.lock();
    m_stop = true;
    m_capture->release();
    delete m_capture;
    m_waitcond.wakeOne();
    m_mutex.unlock();
    wait();
}
