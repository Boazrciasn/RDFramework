#ifndef RECTANGLEPARTICLE_H
#define RECTANGLEPARTICLE_H

#include "Particle.h"
#include "Util.h"

class RectangleParticle : public Particle
{
  public:
    RectangleParticle(int x, int y, int width, int height, float weight, cv::Mat target, int histSize) : Particle(x, y, weight,
                target, histSize)
    {
        m_width = width;
        m_height = height;
    }

    Particle *clone() override
    {
        return new RectangleParticle(m_x, m_y, m_width, m_height ,m_weight, m_TargetHist, m_histSize);
    }


    inline void exec(cv::Mat *img) override
    {
        //        float mean = 0.0;
        // handle out of bounds
        if (m_x + m_width >= img->cols || m_y + m_height >= img->rows){
            setWeight(0); // to discard (we can shrink particle)
            return;
        }

        if (m_TargetHist.size > 0)
        {
            cv::Mat roi(*img, cv::Rect(m_x, m_y, m_width, m_height));
            cv::Mat roiHist;
            Util::CalculateHistogram(roi, roiHist, m_histSize); // TODO: we might use "m_TargetHist.size()" instead of m_histSize
            double distBhat = compareHist(m_TargetHist, roiHist, CV_COMP_CORREL);
            setWeight(distBhat);
        }
    }

    inline cv::Rect getParticle() const {return cv::Rect(m_x, m_y, m_width, m_height); }

    inline int getWidth() const {return m_width;}
    inline void setWidth(int value) {m_width = value;}

    inline int getHeight() const {return m_height;}
    inline void setHeight(int value) {m_height = value;}


  private:
    int m_width;
    int m_height;
};

#endif // RECTANGLEPARTICLE_H
