#ifndef PREPROCESS_H
#define PREPROCESS_H
#include "precompiled.h"
#include "boost/variant.hpp"
typedef std::vector<cv::Mat> images;
class PreProcessor
{
  protected :
    images m_images;

  public:
    // Batch process images (useful for dataset preprocessing).
    template<typename Processor>
    void doBatchPreProcess(images &inputImg, Processor &p)
    {
        for (auto &img : inputImg)
            p.process_img(img);
    }

    // Preprocess single image (for tracking etc...)
    template<typename Processor>
    void doPreProcess(cv::Mat &img, Processor &p)
    {
        p.process_img(img);
    }
};


template<typename ...PTYPES>
class AggregateProcessor : private PTYPES...
{
  private:
    //recursively open variadic template parameters
    template<class First, class...Rest>
    struct process_helper
    {
        static void do_process(AggregateProcessor *pthis, cv::Mat &img)
        {
            static_cast<First *>(pthis)->processImg(img);
            process_helper<Rest...>::do_process(pthis, img);
        }
    };

    template<class First>
    struct process_helper<First>
    {
        static void do_process(AggregateProcessor *pthis, cv::Mat &img)
        {
            static_cast<First *>(pthis)->processImg(img);
        }
    };

  public:
    template<class... P_INITS>
    AggregateProcessor(P_INITS &... p) : PTYPES(p)...
    {
    }

    void process_img(cv::Mat &img)
    {
        process_helper<PTYPES...>::do_process(this, img);
    }

};

//Inverse Background :
class InverseImage
{
  public :
    InverseImage() {}
    InverseImage(InverseImage &refObj) {}
    inline void processImg(cv::Mat img)
    {
        img = 255 - img;
    }
};

//Gaussian Blur :
class Gaussian
{
  public :
    //init with values :
    Gaussian(int kSizeX, int kSizeY, int gSigma) : m_gSigma(gSigma)
    {
        m_gSize = cv::Size(kSizeX, kSizeY);
    }

    //init with reference object :
    Gaussian(Gaussian &refObj) : m_gSigma(refObj.m_gSigma), m_gSize(refObj.m_gSize) {}
    inline void processImg(cv::Mat img)
    {
        cv::GaussianBlur(img, img, m_gSize, m_gSigma);
    }
    inline cv::Size gSize() {return m_gSize;}
    inline int gSigma() {return m_gSigma;}
  private :
    int m_gSigma;
    cv::Size m_gSize;
};

//Extend borders for RDF vectors :
class MakeBorder
{
  public:
    //init with values :
    MakeBorder(int borderX, int borderY, cv::BorderTypes borderType) : m_borderX(borderX), m_borderY(borderY),
        m_borderType(borderType) {}

    //init with reference object
    MakeBorder(MakeBorder &refObj) : m_borderX(refObj.m_borderX), m_borderY(refObj.m_borderY),
        m_borderType(refObj.m_borderType) {}
    inline void processImg(cv::Mat &img)
    {
        cv::copyMakeBorder(img, img, m_borderY, m_borderY, m_borderX, m_borderX, m_borderType);
    }
    int m_borderX;
    int m_borderY;
    cv::BorderTypes m_borderType;

};





#endif // PREPROCESS_H
