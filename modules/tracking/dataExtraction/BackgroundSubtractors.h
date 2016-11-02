
#include "precompiled.h"
#include <opencv2/video/background_segm.hpp>

#include "Util.h"

#ifndef BACKGROUNDSUBTRACTORS_H
#define BACKGROUNDSUBTRACTORS_H

class BackgroundSubMoG
{
  public:
    BackgroundSubMoG(int erosionSize = 5, int dilationSize = 5, int blurSize = 5,
                     cv::MorphShapes morphElem = cv::MORPH_ELLIPSE) :
        m_erosionSize(erosionSize), m_dilationSize(dilationSize), m_blurSize(blurSize), m_morphElem(morphElem)
    {
        m_pMOG = cv::createBackgroundSubtractorMOG2();
        m_pMOG->setShadowValue(0);
    }

    void execute(const cv::Mat &inputImg, cv::Mat &outImg)
    {
        cv::Mat blurredImg;
        cv::blur(inputImg, blurredImg, cv::Size(m_blurSize, m_blurSize));
        m_pMOG->apply(blurredImg, m_fgMask);

        Util::Dilation(m_fgMask, m_fgMask, m_dilationSize, m_morphElem);
        Util::Erosion(m_fgMask, m_fgMask, m_erosionSize, m_morphElem);
        outImg = m_fgMask;
    }


    void setParam(int erosionSize, int dilationSize, int blurSize = 5, cv::MorphShapes morphElem = cv::MORPH_ELLIPSE)
    {
        setErosionSize(erosionSize);
        setDilationSize(dilationSize);
        setBlurSize(blurSize);
        setErosionSize(morphElem);
        m_pMOG = cv::createBackgroundSubtractorMOG2();
        m_pMOG->setShadowValue(0);
    }
    void setErosionSize(int s) { m_erosionSize = s;}
    void setDilationSize(int s) { m_dilationSize = s;}
    void setBlurSize(int s) { m_blurSize = s;}
    void setErosionSize(cv::MorphShapes elem) { m_morphElem = elem;}

  private :
    cv::Ptr<cv::BackgroundSubtractorMOG2> m_pMOG;
    cv::Mat_<uchar> m_fgMask;
    cv::Mat m_frameOut;
    int m_erosionSize;
    int m_dilationSize;
    int m_blurSize;
    cv::MorphShapes m_morphElem = cv::MORPH_ELLIPSE;
    cv::Size m_sturcture_size ; //= cv::Size(2 * m_dilation_size + 1, 2 * m_dilation_size + 1);
    cv::Point m_structure_point;
};

#endif // BACKGROUNDSUBTRACTORS_H
