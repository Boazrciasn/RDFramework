#include "precompiled.h"
#include "VideoReader.h"


VideoReader::VideoReader(QObject *parent, BufferQueue *buffer) : QThread(parent)
{
    m_FrameBuffer = buffer;
}

void VideoReader::run()
{
    while (!m_stop)
    {
        while (getBufferSize() < 200 && m_bufferEndPos <= m_numOfFrames)
        {
            bool capSuccess = m_capture->read(m_frame);
            if (!capSuccess)
                qDebug() << "NULL";
            cv::cvtColor(m_frame, m_RGBframe, CV_BGR2RGB);
            m_FrameBuffer->enqueue(m_RGBframe.clone());
            ++m_bufferEndPos;
            msleep(0);
        }
    }
}

void VideoReader::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}


bool VideoReader::loadVideo(std::string filename)
{
    m_capture = new cv::VideoCapture(filename);
    if (m_capture->isOpened())
    {
        m_frameRate = (int) m_capture->get(CV_CAP_PROP_FPS);
        m_numOfFrames = m_capture->get(CV_CAP_PROP_FRAME_COUNT);
        qDebug() << "file is open!";
        return true;
    }
    else
    {
        qDebug() << "Unable to open video file!";
        return false;
    }
}

void VideoReader::startBuffer()
{
    if (!isRunning())
    {
        if (m_stop)
            m_stop = false;
        start(HighestPriority);
    }
}

int VideoReader::getBufferSize() const
{
    return m_FrameBuffer->getSize();
}

void VideoReader::setCurrentFrame(int frameNumber)
{
    m_FrameBuffer->clear();
    m_bufferEndPos = frameNumber;
    m_capture->set(CV_CAP_PROP_POS_FRAMES, frameNumber);
    startBuffer();
}

VideoReader::~VideoReader()
{
    m_mutex.lock();
    m_stop = true;
    m_capture->release();
    delete m_capture;
    delete m_FrameBuffer;
    m_waitcond.wakeOne();
    m_mutex.unlock();
    wait();
}
