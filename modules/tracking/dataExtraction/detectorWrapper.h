#ifndef DETECTORWRAPPER_H
#define DETECTORWRAPPER_H
#include "precompiled.h"
#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>

class HoGSVM
{
  public:
    HoGSVM()
    {
        m_hog = new cv::HOGDescriptor();
    }

    void compute(const cv::Mat &inputImg, quint32 &decision, quint32 &confidence)
    {
        cv::Mat processed_img;
        cv::cvtColor(inputImg, processed_img, CV_RGB2GRAY);
        //Todo : Sizes constant ?
        cv::resize(processed_img, processed_img, cv::Size(64, 128));
        std::vector<float> descriptor;
        m_hog->compute(processed_img, descriptor, cv::Size(64, 128), cv::Size(16, 16));
        cv::Mat_<float> desc(descriptor);
        decision = m_svm->predict(desc.t(), cv::noArray(), cv::ml::StatModel::RAW_OUTPUT);
        confidence = 1.0 / (1.0 + exp(decision));

    }

    inline void setHOGDescriptor(cv::HOGDescriptor *hog) {m_hog = hog;}
    inline cv::HOGDescriptor *getHOGDescriptor() {return m_hog;}

    inline void setSVM(cv::Ptr<cv::ml::SVM> svm) {m_svm = svm;}
    inline cv::Ptr<cv::ml::SVM> getSVM() {return m_svm;}

    void init() { m_hog = new cv::HOGDescriptor(); }
  private:
    cv::HOGDescriptor *m_hog;
    cv::Ptr<cv::ml::SVM> m_svm;

};
#endif // DETECTORWRAPPER_H
