#ifndef PREPROCESS_H
#define PREPROCESS_H
#include "precompiled.h"

template<typename P>
class Process
{
  public:
    Process() {}

    void process(std::vector<cv::Mat> &images)
    {
        for (auto &img : images)
            p().processImg(img);
    }

  private:
    P &p()
    {
        return *static_cast<P *>(this);
    }
};

//Inverse Background :
class InverseImage : public Process<InverseImage>
{
  public :
    void processImg(cv::Mat img)
    {
        img = 255 - img;
    }
};

//Gaussian Blur :
class Gaussian : public Process<Gaussian>
{
  public :
    Gaussian(int kSizeX, int kSizeY, int gSigma) : m_gSigma(gSigma)
    {
        gSize = cv::Size(kSizeX, kSizeY);
    }
    void processImg(cv::Mat img)
    {
        cv::GaussianBlur(img, img, gSize, m_gSigma);
    }
  private:
    cv::Size gSize;
    int m_gSigma;
};

//Extend borders for RDF vectors :
class MakeBorder : public Process<MakeBorder>
{
  public:
    MakeBorder(int borderX, int borderY, cv::BorderTypes borderType) : m_borderX(borderX), m_borderY(borderY),
        m_borderType(borderType) {}
    void processImg(cv::Mat &img)
    {
        cv::copyMakeBorder(img, img, m_borderY, m_borderY, m_borderX, m_borderX, m_borderType);
    }
  private:
    int m_borderX;
    int m_borderY;
    cv::BorderTypes m_borderType;
};

#endif // PREPROCESS_H
