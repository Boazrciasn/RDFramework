#ifndef PARTICLE_HEADER
#define PARTICLE_HEADER


#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

enum {Circle, Rectangle};

class Particle
{
public:
    Particle(int x, int y, int width, float weight)
	{
		m_x = x;
        m_y = y;
        m_width = width;
		m_weight = weight;
	}

    virtual void exec(cv::Mat *img) = 0;

    inline int x() const { return m_x; }
	inline void setX(int value) { m_x = value; }

    inline int y() const { return m_y; }
	inline void setY(int value) { m_y = value; }

    inline float weight() const { return m_weight; }
    inline void setWeight(float value) { m_weight = value; }

    inline int width() const { return m_width; }
    inline void setWidth(int value) { m_width = value; }

protected:
	int m_x;
	int m_y;
    int m_width;

	float m_weight;
};

#endif
