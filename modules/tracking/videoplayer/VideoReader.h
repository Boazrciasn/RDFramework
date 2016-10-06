#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H
#include "precompiled.h"
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <queue>
#include "Util.h"
#include "BufferQueue.h"
#include "tracking/particlefilter/ParticleFilter.h"

//#include "opencv2/opencv.hpp"  //TODO: move to precompiled

class VideoReader : public QThread
{
    Q_OBJECT

  private:
    QWaitCondition m_waitcond;
    cv::Mat m_frame;
    cv::Mat m_frame_out;
    cv::Mat m_frame_gray;
    double m_frameRate;
    int m_numOfFrames;
    int m_bufferEndPos = 0;
    cv::VideoCapture *m_capture;
    cv::Mat m_RGBframe;
    QImage m_img;
    BufferQueue<cv::Mat> *m_FrameBuffer;
    bool m_stop = false;

  protected:
    void run();
    void msleep(int ms);

  public:
    VideoReader(QObject *parent, BufferQueue<cv::Mat> *buffer);
    ~VideoReader();
    bool loadVideo(std::string filename);
    void startBuffer();
    void stopBuffer() { m_stop = true; }

    void setCurrentFrame(int frameNumber);
    int getBufferSize() const;
    double getFrameRate() const { return m_frameRate; }
    int getNumberOfFrames() const { return m_capture->get(CV_CAP_PROP_FRAME_COUNT); }
    int getFrameHeight() const { return m_capture->get(CV_CAP_PROP_FRAME_HEIGHT); }
    int getFrameWidth() const { return m_capture->get(CV_CAP_PROP_FRAME_WIDTH); }

};

#endif // VIDEOPLAYER_H
