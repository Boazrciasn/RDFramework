#ifndef PARTICLE_HEADER
#define PARTICLE_HEADER


#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

enum {Circle, Rectangle};

class Particle
{
  public:
    Particle(int x, int y, float weight, cv::Mat &target, int histSize)
    {
        m_x = x;
        m_y = y;
        m_weight = weight;
        m_TargetHist = target;
        m_histSize = histSize;
    }

    virtual void exec(cv::Mat *img) = 0;
    virtual Particle *clone() = 0;

    inline int getWidth() const {return m_width;}
    inline int getX() const {return m_x;}
    inline int getY() const {return m_y;}
    inline float getWeight() const {return m_weight;}

    inline void setX(int value) {m_x = value;}
    inline void setY(int value) {m_y = value;}
    inline void setWidth(int value) {m_width = value;}
    inline void setWeight(float value) { m_weight = value; }


  protected:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    int m_histSize;
    float m_weight;
    cv::Mat m_TargetHist;

};

#endif
