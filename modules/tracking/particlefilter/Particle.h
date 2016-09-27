#ifndef PARTICLE_HEADER
#define PARTICLE_HEADER


#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>

enum {Circle, Rectangle};

class Particle
{
  public:
    Particle(int x, int y, float weight, cv::Mat &target, int histSize)
    {
        m_x = x;
        m_y = y;
        m_weight = weight;
        m_targetHist = target;
        m_histSize = histSize;
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

    inline cv::Mat target() const {return m_targetHist;}
    inline void setTarget(cv::Mat &target) {m_targetHist = target;}

    inline int histSize() const {return m_histSize;}
    inline void setHistSize(int value) {m_histSize = value;}

    inline void setHOGDescriptor(cv::HOGDescriptor *hog){m_hog = hog;}
    inline cv::HOGDescriptor* getHOGDescriptor(){return m_hog;}

    inline void setSVM(cv::Ptr<cv::ml::SVM> svm){m_svm = svm;}
    inline cv::Ptr<cv::ml::SVM> getSVM(){return m_svm;}

  protected:
    int m_x;
    int m_y;
    int m_histSize;
    float m_weight;
    cv::Mat m_targetHist;

    cv::HOGDescriptor *m_hog;
    cv::Ptr<cv::ml::SVM> m_svm;
};

#endif
