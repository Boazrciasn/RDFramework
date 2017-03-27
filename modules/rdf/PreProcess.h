#ifndef PREPROCESS_H
#define PREPROCESS_H
#include "precompiled.h"

typedef std::vector<cv::Mat> images;

//Dummy?
class Process
{
  public:
    virtual void processImg(cv::Mat &img) = 0;
};


//Inverse Background :
class InverseImage : public Process
{
  public :
    InverseImage() {}
    InverseImage(InverseImage &refObj) {}
    void processImg(cv::Mat &img)
    {
        img = 255 - img;
    }
};


//Gaussian Blur :
class Gaussian : public Process
{
  public :
    //init with values :
    Gaussian(int kSizeX, int kSizeY, int gSigma) : m_gSigma(gSigma)
    {
        m_gSize = cv::Size(kSizeX, kSizeY);
    }

    //init with reference object :
    Gaussian(Gaussian &refObj) : m_gSigma(refObj.m_gSigma), m_gSize(refObj.m_gSize) {}

    //Default constructor :
    Gaussian() {}

    void setParam(int kSizeX, int kSizeY, int gSigma)
    {
        m_gSigma = gSigma;
        m_gSize = cv::Size(kSizeX, kSizeY);
    }

    void processImg(cv::Mat &img)
    {
        cv::GaussianBlur(img, img, m_gSize, m_gSigma);
    }

    inline cv::Size gSize() {return m_gSize;}
    inline int gSigma() {return m_gSigma;}
  private :
    int m_gSigma;
    cv::Size m_gSize;
};

//Sobel :
class Sobel : public Process
{
  public :
    //init with values :
    Sobel(int kSizeX, int kSizeY, int sigma) : m_sigma(sigma)
    {
        m_size = cv::Size(kSizeX, kSizeY);
    }

    //init with reference object :
    Sobel(Sobel &refObj) : m_sigma(refObj.m_sigma), m_size(refObj.m_size) {}

    //Default constructor :
    Sobel() {}

    void setParam(int kSizeX, int kSizeY, int sigma)
    {
        m_sigma = sigma;
        m_size = cv::Size(kSizeX, kSizeY);
    }

    void setKSize(int ksize)
    {
        m_ksize = ksize; // CV_SCHARR for Scharr
    }

    void processImg(cv::Mat &img)
    {
        m_ksize = CV_SCHARR;
        /// Generate grad_x and grad_y
        cv::Mat grad_x, grad_y;
        cv::Mat abs_grad_x, abs_grad_y;

        cv::GaussianBlur(img, img, m_size, m_sigma);
        /// Gradient X
        cv::Sobel(img, grad_x, m_ddepth, 1, 0, m_ksize, m_scale, m_delta, cv::BORDER_DEFAULT);
        cv::convertScaleAbs(grad_x, abs_grad_x);
        /// Gradient Y
        cv::Sobel(img, grad_y, m_ddepth, 0, 1, m_ksize, m_scale, m_delta, cv::BORDER_DEFAULT);
        cv::convertScaleAbs(grad_y, abs_grad_y);
        /// Total Gradient (approximate)
        cv::addWeighted(abs_grad_x, m_alpha, abs_grad_y, m_beta, m_gamma, img);
    }

  private :
    int m_sigma;
    cv::Size m_size;

    double m_alpha = 0.5;
    double m_beta = 0.5;
    double m_gamma = 0;
    int m_ksize = 3;
    int m_scale = 1;
    int m_delta = 0;
    int m_ddepth = CV_16S;
};


//Extend borders for RDF vectors :
class MakeBorder : public Process
{
  public:
    //init with values :
    MakeBorder(int borderX, int borderY, cv::BorderTypes borderType) : m_borderX(borderX), m_borderY(borderY),
        m_borderType(borderType) {}

    //init with reference object
    MakeBorder(MakeBorder &refObj) : m_borderX(refObj.m_borderX), m_borderY(refObj.m_borderY),
        m_borderType(refObj.m_borderType) {}

    void processImg(cv::Mat &img)
    {
        cv::copyMakeBorder(img, img, m_borderY, m_borderY, m_borderX, m_borderX, m_borderType);
    }

    void setParam(int borderX, int borderY, cv::BorderTypes borderType)
    {
        m_borderX = borderX;
        m_borderY = borderY;
        m_borderType = borderType;
    }
    //Default constructor :
    MakeBorder() {}
    int m_borderX;
    int m_borderY;
    cv::BorderTypes m_borderType;

};

class PreProcess
{

  public:
    void setPreProcess(std::vector<Process *> &p) { m_processes = p;}

    static void doBatchPreProcess(images &inputImg, std::vector<Process *> processes)
    {
        for (auto &img : inputImg)
            for (auto &p : processes)
                p->processImg(img);
    }

    void doBatchPreProcess(images &inputImg)
    {
        for (auto &img : inputImg)
            for (auto &p : m_processes)
                p->processImg(img);
    }

    void doPreProcess(cv::Mat &img)
    {
        for (auto &p : m_processes)
            p->processImg(img);
    }

    static void doBatchPreProcessSingle(cv::Mat &img, std::vector<Process *> processes)
    {
        for (auto &p : processes)
            p->processImg(img);
    }

  private:
    std::vector<Process *> m_processes;
};




//typedef std::vector<cv::Mat> images;
//class PreProcessor
//{
//  public:
//    // Batch process images (useful for dataset preprocessing).
//    template<typename Processor>
//    void doBatchPreProcess(images &inputImg, Processor &p)
//    {
//        for (auto &img : inputImg)
//            p.process_img(img);
//    }

//    // Preprocess single image (for tracking etc...)
//    template<typename Processor>
//    void doPreProcess(cv::Mat &img, Processor &p)
//    {
//        p.process_img(img);
//    }
//};


//template<typename ...PTYPES>
//class AggregateProcessor : private PTYPES...
//{
//  private:

//    //recursively open variadic template parameters
//    template<class First, class...Rest>
//    struct process_helper
//    {
//        static void do_process(AggregateProcessor *pthis, cv::Mat &img)
//        {
//            static_cast<First *>(pthis)->processImg(img);
//            process_helper<Rest...>::do_process(pthis, img);
//        }
//    };

//    template<class First>
//    struct process_helper<First>
//    {
//        static void do_process(AggregateProcessor *pthis, cv::Mat &img)
//        {
//            static_cast<First *>(pthis)->processImg(img);
//        }
//    };

//  public:
//    template<class... P_INITS>
//    AggregateProcessor(P_INITS &... p) : PTYPES(p)...
//    {
//    }

//    void process_img(cv::Mat &img)
//    {
//        process_helper<PTYPES...>::do_process(this, img);
//    }

//};







#endif // PREPROCESS_H
