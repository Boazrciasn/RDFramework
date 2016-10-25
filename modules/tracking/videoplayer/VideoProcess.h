#include "precompiled.h"

#ifndef VIDEOPROCESS_H
#define VIDEOPROCESS_H

class VideoProcess
{
  public :
    virtual void exec(const cv::Mat &img, cv::Mat &imgOut)
    {
        imgOut = img;
    }
    void setImage(const cv::Mat img)
    {
        m_img = img;
    }

  protected :
    cv::Mat m_img;
};

#endif // VIDEOPROCESS_H
