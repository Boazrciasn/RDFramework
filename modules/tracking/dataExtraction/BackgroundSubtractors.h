#include "precompiled.h"
#include <opencv2/video/background_segm.hpp>

#include "util.h"

#ifndef BACKGROUNDSUBTRACTORS_H
#define BACKGROUNDSUBTRACTORS_H

class BackgroundSubMoG
{
  public:
    BackgroundSubMoG(int erosion_size, int dilation_size, int blurSize = 5,
                     cv::MorphShapes morph_elem = cv::MORPH_ELLIPSE) :
        m_erosion_size(erosion_size), m_dilation_size(dilation_size), m_blurSize(blurSize), m_morph_elem(morph_elem)
    {
    }
    cv::Mat execute(cv::Mat inputImg)
    {
        cv::blur(inputImg, inputImg, cv::Size(m_blurSize, m_blurSize));
        m_pMOG->apply(inputImg, m_fgMaskMOG2);
        Util::Dilation(m_fgMaskMOG2, m_frameOut, m_dilation_size, m_morph_elem);
        Util::Erosion(m_frameOut, m_frameOut, m_dilation_size, m_morph_elem);
        return m_frameOut;
    }
  private :
    cv::Ptr<cv::BackgroundSubtractorMOG2> m_pMOG;
    cv::Mat_<uchar> m_fgMaskMOG2;
    cv::Mat m_frameOut;
    int m_erosion_size;
    int m_dilation_size;
    int m_blurSize;
    cv::MorphShapes m_morph_elem = cv::MORPH_ELLIPSE;
    cv::Size m_sturcture_size ; //= cv::Size(2 * m_dilation_size + 1, 2 * m_dilation_size + 1);
    cv::Point m_structure_point;
};

#endif // BACKGROUNDSUBTRACTORS_H
