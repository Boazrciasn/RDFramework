#include "precompiled.h"
#include "VideoPlayer.h"
#include "Util.h"


VideoPlayer::VideoPlayer(QObject *parent): QThread(parent)
{
    m_FrameBuffer = new BufferQueue<cv::Mat>();
    m_VideoReader = new VideoReader(this, m_FrameBuffer);
    m_pMOG = cv::createBackgroundSubtractorMOG2();
    m_pMOG->setShadowValue(0);
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
        if (m_PF)
            processPF();
        else
            m_img = Util::toQt(m_RGBframe, QImage::Format_RGB888);
        emit playerFrame(m_img);

        cv::imshow("FG Mask fgMOG_no_hole", m_fgMaskMOG2_noHole);
        cv::imshow("FG Mask", m_fgMaskMOG2);
        msleep(delay);
    }
}

void VideoPlayer::processPF()
{
    // Background Subtruction MOG
    cv::blur(m_RGBframe, m_RGBframe, cv::Size(5, 5));
    m_pMOG->apply(m_RGBframe,m_fgMaskMOG2);

//    std::cout << m_fgMaskMOG2 << std::endl;

    int pointCount = cv::sum(m_fgMaskMOG2)[0]/255 + 1;  // +1 for dummy first elem
    int index = 1;
    std::vector<cv::Point> fgPixs(pointCount);

    doForAllPixels(m_fgMaskMOG2,[&](uchar value, int x, int y)
    {
        value = value/255;
        fgPixs[index*value] = cv::Point(x,y);
        index += value;
    });

    fgPixs.erase(fgPixs.begin()); // remove dummy

    std::vector<std::vector<cv::Point> > clusters = Util::DBSCAN_points(&fgPixs,40.0,1);
//    std::cout << clusters.size() << std::endl;

    // Default start
    Dilation( 0, 0 );
    Erosion( 0, 0 );
    m_PF->setIMG(&m_RGBframe);
    m_PF->processImage();
    m_frame_out = m_PF->getIMG();
    m_img = Util::toQt(m_frame_out, QImage::Format_RGB888);
}

/**  @function Erosion  */
void VideoPlayer::Erosion( int, void* )
{
  int erosion_type;

  if( m_erosion_elem == 0 ){ erosion_type = cv::MORPH_RECT; }
  else if( m_erosion_elem == 1 ){ erosion_type = cv::MORPH_CROSS; }
  else if( m_erosion_elem == 2) { erosion_type = cv::MORPH_ELLIPSE; }


//  erosion_type = cv::MORPH_RECT;
  cv::Mat element = cv::getStructuringElement( erosion_type,
                                       cv::Size( 2*m_erosion_size + 1, 2*m_erosion_size+1 ),
                                       cv::Point( m_erosion_size, m_erosion_size ) );

  /// Apply the erosion operation
  cv::erode( m_fgMaskMOG2_dilated, m_fgMaskMOG2_noHole, element );
}

/** @function Dilation */
void VideoPlayer::Dilation( int, void* )
{
  int dilation_type;
  if( m_dilation_elem == 0 ){ dilation_type = cv::MORPH_RECT; }
  else if( m_dilation_elem == 1 ){ dilation_type = cv::MORPH_CROSS; }
  else if( m_dilation_elem == 2) { dilation_type = cv::MORPH_ELLIPSE; }


//  dilation_type = cv::MORPH_RECT;
  cv::Mat element = cv::getStructuringElement( dilation_type,
                                       cv::Size( 2*m_dilation_size + 1, 2*m_dilation_size+1 ),
                                       cv::Point( m_dilation_size, m_dilation_size ) );
  /// Apply the dilation operation
  cv::dilate( m_fgMaskMOG2, m_fgMaskMOG2_dilated, element );
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
    delete m_VideoReader;
    m_waitcond.wakeOne();
    wait();
}
