#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include "modules/tracking/videoplayer/VideoProcess.h"
#include "util.h"

template <typename BSubtractor, typename PFinder>
class DataExtractor : public VideoProcess
{
  public:
    DataExtractor (BSubtractor &bsubtract, const PFinder &pfinder) :
        m_bsubtractor(bsubtract), m_pfinder(pfinder)
    {
    }

    void exec(const cv::Mat &inputImg, cv::Mat &outImg)
    {
        setImage(inputImg);
        m_img = inputImg;
        cv::Mat fg_img;
        m_bsubtractor.execute(m_img, fg_img);
        std::vector<cv::Rect>  bBox_vec;
        bBox_vec = Util::calculateBoundingBoxRect(fg_img, 300, 4000);
        Util::drawBoundingBox(m_img, bBox_vec);
        outImg = m_img;
    }


  private:
    BSubtractor &m_bsubtractor;
    const PFinder &m_pfinder;
    cv::Mat m_img;
};

#endif // DATAEXTRACTOR_H
