#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include "particlefilter/SimplePF.h"
#include <queue>


//#include "opencv2/opencv.hpp"  //TODO: move to precompiled

#include "Util.h"

class VideoReader : public QThread
{
    Q_OBJECT

  private:
    bool m_stop;
    QMutex m_mutex;
    QWaitCondition m_waitcond;
    cv::Mat m_frame;
    cv::Mat m_frame_out;
    cv::Mat m_frame_gray;
    int m_frameRate;
    cv::VideoCapture *m_capture;
    cv::Mat m_RGBframe;
    QImage m_img;

    std::deque<cv::Mat> m_frame_buffer;

    void addToBuffer(cv::Mat frame);
    void processFrame();

  signals :
    void processedImage(const QImage &image);

  protected:
    void run();
    void msleep(int ms);

  public:
    VideoReader(QObject *parent);
    ~VideoReader();
    bool m_bufferReady = false;
    bool loadVideo(std::string filename);
    void loadBuffer();
    void stopVideo();
    bool isStopped() const;
    int getBufferSize();

//    inline void setPF(SimplePF *pf){ mPF = pf;}
//    inline SimplePF* getPF(){ return mPF;}

    void setCurrentFrame(int frameNumber);
    double getFrameRate();
    double getCurrentFrame();
    int getNumberOfFrames();
    int getFrameHeight();
    int getFrameWidth();

    cv::Mat getFrame();
    void getFrame(cv::Mat &frame, int index);
};

#endif // VIDEOPLAYER_H
