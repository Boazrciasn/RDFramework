#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include "SimplePF.h"
//#include "opencv2/opencv.hpp"  //TODO: move to precompiled

#include "Util.h"

class VideoPlayer : public QThread
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

    SimplePF *mPF;

    void processFrame();

  signals :
    void processedImage(const QImage &image);

  protected:
    void run();
    void msleep(int ms);

  public:
    VideoPlayer(QObject *parent);
    ~VideoPlayer();
    bool loadVideo(std::string filename);
    void playVideo();
    void stopVideo();
    bool isStopped() const;

    inline void setPF(SimplePF *pf){ mPF = pf;}
    inline SimplePF* getPF(){ return mPF;}

    void setCurrentFrame(int frameNumber);
    double getFrameRate();
    double getCurrentFrame();
    double getNumberOfFrames();
    double getFrameHeight();
    double getFrameWidth();
};

#endif // VIDEOPLAYER_H
