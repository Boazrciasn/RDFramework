#include "precompiled.h"
#include "VideoReader.h"

VideoReader::VideoReader(QObject *parent) : QThread(parent)
{
    m_stop = true;
}

void VideoReader::run()
{
//    int delay = (1000 / m_frameRate);
    while(!m_stop)
    {
        bool capSuccess = m_capture->read(m_frame);
        if (!capSuccess)
        {
            m_stop = true;
            qDebug() << "NULL";
        }
        cv::cvtColor(m_frame, m_RGBframe, CV_BGR2RGB);
        addToBuffer(m_RGBframe.clone());

//        m_img = Util::toQt(m_RGBframe,QImage::Format_RGB888);

//        if(mPF != NULL)
//            processFrame();
//        emit processedImage(m_img);
        msleep(0);
    }
}

void VideoReader::addToBuffer(cv::Mat frame){
    m_mutex.lock();
    m_frame_buffer.push_back(frame);
    m_mutex.unlock();
}

cv::Mat VideoReader::getFrame(){
    cv::Mat out;
    m_mutex.lock();
    qDebug()<<m_frame_buffer.size() << "Test size";
    if(!m_frame_buffer.empty())
    {
        out = m_frame_buffer.front();
        m_frame_buffer.pop_front();
    }

    qDebug()<<m_frame_buffer.size() << "after Test size";
    m_mutex.unlock();
    return out;
}

void VideoReader::processFrame()
{
//    cv::cvtColor(m_RGBframe, m_frame_gray, CV_BGR2GRAY);
//    mPF->setIMG(&m_frame_gray);
//    mPF->run();
//    m_frame_out = mPF->getIMG();
//    m_img = Util::toQt(m_frame_out,QImage::Format_RGB888);
}

void VideoReader::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}

bool VideoReader::loadVideo(std::string filename)
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

void VideoReader::playVideo()
{
    if(!isRunning())
    {
        if(isStopped())
            m_stop = false;

        start(LowPriority);
    }
}

void VideoReader::stopVideo()
{
    m_stop = true;
}

bool VideoReader::isStopped() const
{
    return m_stop;
}

void VideoReader::setCurrentFrame(int frameNumber)
{
    m_capture->set(CV_CAP_PROP_POS_FRAMES, frameNumber);
}

double VideoReader::getFrameRate()
{
    return m_frameRate;
}

double VideoReader::getCurrentFrame()
{
    return m_capture->get(CV_CAP_PROP_POS_FRAMES);
}

int VideoReader::getNumberOfFrames()
{
    return m_capture->get(CV_CAP_PROP_FRAME_COUNT);
}

int VideoReader::getFrameHeight()
{
    return m_capture->get(CV_CAP_PROP_FRAME_HEIGHT);
}

int VideoReader::getFrameWidth()
{
    return m_capture->get(CV_CAP_PROP_FRAME_WIDTH);
}


VideoReader::~VideoReader()
{
    m_mutex.lock();
    m_stop = true;
    m_capture->release();
    delete m_capture;
    delete &m_frame_buffer;
    m_waitcond.wakeOne();
    m_mutex.unlock();
    wait();
}
