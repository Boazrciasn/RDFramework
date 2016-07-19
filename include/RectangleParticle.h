#ifndef RECTANGLEPARTICLE_H
#define RECTANGLEPARTICLE_H

#include "Particle.h"

class RectangleParticle : public Particle
{
public:
    RectangleParticle(int x, int y, int width, float weight) : Particle(x,y,width,weight){}

    inline void exec(cv::Mat *img)
    {
        float mean = 0.0;

        for (int row = m_y; row < m_y + m_width; row++)
            for (int col = m_x; col < m_x + m_width; col++)
                mean += img->at<uchar>(row, col);

        mean = 255 -mean / (m_width*m_width);
        setWeight(mean);
    }
};

#endif // RECTANGLEPARTICLE_H
