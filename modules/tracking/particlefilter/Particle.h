#ifndef PARTICLE_HEADER
#define PARTICLE_HEADER


#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>
#include "rdf/RDFBasic.h"

enum {CIRCLE, RECTANGLE};

class Particle
{
  public:
    Particle(int x, int y, float weight)
    {
        m_x = x;
        m_y = y;
        m_weight = weight;
    }

    virtual void exec(cv::Mat *img) = 0;
    virtual Particle *clone() = 0;

    inline cv::Point getCoordinates() const {return cv::Point(m_x,m_y);}
    inline void setCoordinates(cv::Point coord)
    {
        m_x = coord.x;
        m_y = coord.y;
    }

    inline int x() const { return m_x; }
    inline void setX(int value) {m_x = value;}

    inline int y() const { return m_y; }
    inline void setY(int value) {m_y = value;}

    inline float weight() const {return m_weight;}
    inline void setWeight(float value) { m_weight = value; }

  protected:
    int m_x;
    int m_y;
    float m_weight;
};

#endif
