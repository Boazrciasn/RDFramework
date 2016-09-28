#include "precompiled.h"

#ifndef VIDEOPROCESS_H
#define VIDEOPROCESS_H

class VideoProcess
{
  public :
    VideoProcess() {}
    virtual cv::Mat exec() { return m_img; }
    void setImage(cv::Mat img) { m_img = img; }

  protected :
    cv::Mat m_img;
};

#endif // VIDEOPROCESS_H
