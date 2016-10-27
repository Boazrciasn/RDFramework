#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include "modules/tracking/videoplayer/VideoProcess.h"
#include "util.h"

template <typename BSubtractor, typename PFinder>
class DataExtractor : public VideoProcess
{
  public:
    DataExtractor(BSubtractor &bsubtract, const PFinder &pfinder) :
        m_bsubtractor(bsubtract), m_pfinder(pfinder)
    {
    }

    void exec(const cv::Mat &inputImg, cv::Mat &outImg)
    {
        setImage(inputImg);
        cv::Mat fg_img;
        m_bsubtractor.execute(m_img, fg_img);
        std::vector<cv::Rect>  bBox_vec;
        bBox_vec = Util::calculateBoundingBoxRect(fg_img, m_BBoxMinSize, m_BBoxMaxSize);

        Util::drawBoundingBox(m_img, bBox_vec);
        outImg = m_img;
    }

    void setBBoxSizeConstraints(quint16 minSize, quint16 maxSize)
    {
        m_BBoxMaxSize = maxSize;
        m_BBoxMinSize = minSize;
    }

  private:
    BSubtractor &m_bsubtractor;
    const PFinder &m_pfinder;
    quint16 m_BBoxMinSize;
    quint16 m_BBoxMaxSize;
};

#endif // DATAEXTRACTOR_H
