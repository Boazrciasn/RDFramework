#include "precompiled.h"

#ifndef VIDEOPROCESS_H
#define VIDEOPROCESS_H

class VideoProcess
{
  public :
    virtual void exec(cv::Mat *img) {setImage(img);}
    void setImage(cv::Mat *img) { m_img = img; }
  protected :
    cv::Mat *m_img;
};

#endif // VIDEOPROCESS_H
