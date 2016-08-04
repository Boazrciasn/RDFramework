#ifndef TARGET_H
#define TARGET_H
#include "src/util/precompiled.h"
#include "Util.h"


class Target
{
  public:
    Target(QString label, QImage image, int histSize);
    void setHistSize(int histSize);
    void setLabel(QString label);
    void setImage( QImage image);
    inline QString getLabel()
    {
        return m_label;
    }

    inline cv::Mat getImage()
    {
        return m_targetImg;
    }

    inline cv::Mat getHist()
    {
        return m_targetHist;
    }


    ~Target();

  private:
    int m_histSize;
    QString m_label;
    cv::Mat m_targetImg;
    cv::Mat m_targetHist;

};

#endif // TARGET_H
