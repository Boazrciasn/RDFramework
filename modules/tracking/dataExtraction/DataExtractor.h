#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include "modules/tracking/videoplayer/VideoProcess.h"
#include "util.h"

template <typename BSubtractor, typename Detector>
class DataExtractor : public VideoProcess
{
  public:
    DataExtractor(BSubtractor &bsubtract, Detector &detector) :
        m_bsubtractor(bsubtract), m_detector(detector)
    {
    }

    void exec(const cv::Mat &inputImg, cv::Mat &outImg)
    {
        setImage(inputImg);
        cv::Mat fg_img;
        m_bsubtractor.execute(m_img, fg_img);
        std::vector<cv::Rect>  bBox_vec;
        bBox_vec = Util::calculateBoundingBoxRect(fg_img, m_BBoxMinSize, m_BBoxMaxSize);
        quint32 decision;
        quint32 confidence;
        for (size_t i = 0; i < bBox_vec.size() ; ++i)
        {
            cv::Mat roi(inputImg, bBox_vec[i]);
            m_detector.compute(roi, decision, confidence);
            if (decision == -1.0f)
                bBox_vec.erase(bBox_vec.begin() + i);
        }
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
    Detector &m_detector;
    quint16 m_BBoxMinSize;
    quint16 m_BBoxMaxSize;

};

#endif // DATAEXTRACTOR_H
