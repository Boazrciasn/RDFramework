#ifndef PFEXECUTOR_H
#define PFEXECUTOR_H

#include <QMutex>
#include <QThread>
#include "VideoReader.h"

class PFExecutor : public QThread
{
    Q_OBJECT

public:
  PFExecutor(QObject *parent);
  ~PFExecutor();

signals :
  void processedImage(const QImage &image);

protected:
  void run();
  void msleep(int ms);

private:
  VideoReader* m_video_reader;
  bool m_stop;
  QMutex m_mutex;
  QWaitCondition m_waitcond;
  cv::Mat m_frame_out;
  cv::Mat m_frame_gray;
  int m_frameRate;
  cv::Mat m_RGBframe;
  QImage m_img;

  double m_current_frame;
  double m_numOfFrames;

  SimplePF *mPF;
  void processFrame();

public:
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

#endif // PFEXECUTOR_H
