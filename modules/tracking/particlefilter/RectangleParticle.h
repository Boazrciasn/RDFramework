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
        //        float mean = 0.0;
        // handle out of bounds
        if (m_x + m_width >= img->cols || m_y + m_height >= img->rows) {
            setWeight(0); // to discard (we can shrink particle)
            return;
        }

        cv::Mat roi(*img, cv::Rect(m_x, m_y, m_width, m_height));
        cv::cvtColor(roi, roi, CV_RGB2GRAY);

//        if (m_targetHist.size > 0)
//        {
//            // old way
//            cv::Mat roiHist;
//            Util::CalculateHistogram(roi, roiHist, m_histSize); // TODO: we might use "m_TargetHist.size()" instead of m_histSize
//            float distBhat = compareHist(m_targetHist, roiHist, CV_COMP_CORREL);
//            setWeight(distBhat);
//        }

        cv::resize(roi,roi,cv::Size(64,128));
        std::vector<float> descriptor;
        m_hog->compute(roi, descriptor, cv::Size(64, 128), cv::Size(16, 16));
        // TODO: using descriptor and svm calculate weight for the particle

        cv::Mat_<float> desc(descriptor);
        float decision = m_svm->predict(desc.t(),cv::noArray(), cv::ml::StatModel::RAW_OUTPUT);
        float confidence = 1.0 / (1.0 + exp(decision));
        if(decision > 0)
            setWeight(0.01);
        else
            setWeight(confidence);

        desc.release();
        descriptor.clear();
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
