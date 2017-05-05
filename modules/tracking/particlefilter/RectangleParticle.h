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
        RectangleParticle clone(m_x, m_y, m_width, m_height, m_weight);
        clone.setIsVisited(isVisited());
        return clone;
    }


    void exec(const cv::Mat& img)
    {
        auto conf = 0.0f;
        for (int row = m_y; row < m_y+m_height; ++row)
            for (int col = m_x; col < m_x + m_width ; ++col)
                conf +=  img.at<uchar>(row,col);

        setWeight(conf);
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


    inline bool isVisited() const {return m_isVisited;}
    inline void setIsVisited(bool value) { m_isVisited = value; }


private:
    quint16 m_x{};
    quint16 m_y{};
    quint16 m_width{};
    quint16 m_height{};
    float m_weight{};
    bool m_isVisited{};
};

#endif // RECTANGLEPARTICLE_H
