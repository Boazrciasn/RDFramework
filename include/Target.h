#ifndef TARGET_H
#define TARGET_H
#include "src/precompiled.h"
#include "Util.h"


class Target
{
  public:
    Target(QString label, QImage image);
    void setLabel(QString label);
    void setImage( QImage image);
    QString getLabel();
    cv::Mat getImage();


    ~Target();

  private:
    QString m_Label;
    cv::Mat m_TargetImg;
    cv::Mat m_TargetHist;

};

#endif // TARGET_H
