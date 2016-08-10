#ifndef PFEXECUTOR_H
#define PFEXECUTOR_H

#include <QMutex>
#include <QThread>
#include <tuple>

#include "VideoReader.h"
#include "particlefilter/ParticleFilter.h"
#include "BufferQueue.h"

class VideoPlayer : public QThread
{
    Q_OBJECT

  signals :
    void playerFrame(const QImage &image);

  protected:
    void run();
    void msleep(int ms);

  private:
    VideoReader *m_VideoReader;
    BufferQueue<cv::Mat> *m_FrameBuffer;
    bool m_stop;
    QMutex m_mutex;
    QWaitCondition m_waitcond;
    cv::Mat m_frame_out;
    double m_frameRate;
    cv::Mat m_RGBframe;
    QImage m_img;

    double m_CurrentFrame;

    ParticleFilter *m_PF{};
    void processPF();

    void initializeVideo();

  public:
    VideoPlayer(QObject *parent);
    ~VideoPlayer();

    bool loadVideo(std::string filename);
    void playVideo();
    void stopVideo() { m_stop = true; }
    bool isStopped() const { return m_stop; }
    double getCurrentFrame();
    inline ParticleFilter *getPF() const { return m_PF; }

    int getNumberOfFrames() const { return m_VideoReader->getNumberOfFrames(); }
    std::tuple<int, int> getFrameSize();

    inline void setPF(ParticleFilter *pf) { m_PF = pf; }
    void setCurrentFrame(int frameNumber);
};

#endif // PFEXECUTOR_H
