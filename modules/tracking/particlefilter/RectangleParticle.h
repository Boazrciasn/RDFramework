#ifndef RECTANGLEPARTICLE_H
#define RECTANGLEPARTICLE_H

#include "Particle.h"
#include "Util.h"

class RectangleParticle : public Particle
{
public:
    RectangleParticle(int x, int y, int width, int height, float weight, cv::Mat target, int histSize) :
        Particle(x, y, weight, target, histSize)
    {
        m_width = width;
        m_height = height;
    }

    Particle *clone() override
    {
        return new RectangleParticle(m_x, m_y, m_width, m_height, m_weight, m_targetHist, m_histSize);
    }


    void exec(cv::Mat *img) override
    {
        if (m_x + m_width >= img->cols || m_y + m_height >= img->rows) {
            setWeight(0); // to discard (we can shrink particle)
            return;
        }

//        cv::Mat roi(*img, cv::Rect(m_x, m_y, m_width, m_height));
//        cv::cvtColor(roi, roi, CV_RGB2GRAY);
//        cv::resize(roi,roi,cv::Size(100,100));

        int label{};
        float conf{};

//        m_forest->detect(roi,label,conf);

        if(label == 0)
            setWeight(conf);
        else
            setWeight(0.01);
//        if(label == 1)
//            qDebug() << label << " conf: " << conf;
    }

    inline cv::Rect getParticle() const { return cv::Rect(m_x, m_y, m_width, m_height); }

    inline int getWidth() const {return m_width;}
    inline void setWidth(int value) {m_width = value;}

    inline int getHeight() const {return m_height;}
    inline void setHeight(int value) {m_height = value;}


private:
    int m_width;
    int m_height;
};

#endif // RECTANGLEPARTICLE_H
