#ifndef RECTANGLEPARTICLE_H
#define RECTANGLEPARTICLE_H

#include "Particle.h"
#include "Util.h"

class RectangleParticle : public Particle
{
  public:
    RectangleParticle(int x, int y, int width, float weight, cv::Mat target, int histSize) : Particle(x, y, width, weight,
                target, histSize) {}

    Particle *clone() override
    {
        return new RectangleParticle(m_x, m_y, m_width, m_weight ,m_TargetHist, m_histSize);
    }

    inline void exec(cv::Mat *img) override
    {
        //        float mean = 0.0;
        // handle out of bounds
        if(m_x + m_width >= img->cols || m_y + m_width >= img->rows)
            return;
        if( m_TargetHist.size > 0)
        {
            cv::Mat roi(*img, cv::Rect(m_x, m_y, m_width, m_width));
            cv::Mat roiHist;
            Util::CalculateHistogram(roi, roiHist, m_histSize);
            double distBhat = compareHist(m_TargetHist, roiHist, CV_COMP_BHATTACHARYYA);
            setWeight(distBhat);
        }
        //        mean = cv::sum(roi)[0];
        //        mean = 255 - mean / (m_width * m_width);
        //                for (int row = m_y; row < m_y + m_width; row++)
        //                    for (int col = m_x; col < m_x + m_width; col++)
        //                        mean += img->at<uchar>(row, col);
    }

};

#endif // RECTANGLEPARTICLE_H
