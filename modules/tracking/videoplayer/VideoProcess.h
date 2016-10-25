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
        cv::Mat timg = img.clone();
        m_img = timg;
    }

  protected :
    cv::Mat m_img;
};

#endif // VIDEOPROCESS_H
