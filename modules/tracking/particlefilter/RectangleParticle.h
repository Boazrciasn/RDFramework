#ifndef RECTANGLEPARTICLE_H
#define RECTANGLEPARTICLE_H

#include "precompiled.h"

class RectangleParticle
{
public:
    RectangleParticle() {}

    RectangleParticle(quint16 x, quint16 y, quint16 width, quint16 height, float weight) :
        m_x(x), m_y(y), m_width(width), m_height(height), m_weight(weight) { }

    RectangleParticle clone()
    {
        return RectangleParticle(m_x, m_y, m_width, m_height, m_weight);
    }


    void exec(const cv::Mat& img)
    {
        float conf{};
        for (int row = m_y; row < m_y+m_height; ++row)
            for (int col = m_x; col < m_x + m_width ; ++col)
                conf +=  (img.at<cv::Vec3b>(row,col) == cv::Vec3b(0,0,255) ? 1:0);

        auto tot = m_width*m_height;
        if(conf > tot/2)
            setWeight(conf/tot);
        else
            setWeight(0.0f);
    }

    inline cv::Rect getParticle() const { return cv::Rect(m_x, m_y, m_width, m_height); }

    inline int getWidth() const {return m_width;}
    inline void setWidth(int value) {m_width = value;}

    inline int getHeight() const {return m_height;}
    inline void setHeight(int value) {m_height = value;}

    inline int x() const { return m_x; }
    inline void setX(int value) {m_x = value;}

    inline int y() const { return m_y; }
    inline void setY(int value) {m_y = value;}

    inline float weight() const {return m_weight;}
    inline void setWeight(float value) { m_weight = value; }


private:
    quint16 m_x;
    quint16 m_y;
    quint16 m_width;
    quint16 m_height;
    float m_weight;
};

#endif // RECTANGLEPARTICLE_H
