#ifndef CPV_UTIL
#define CPV_UTIL

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QImage>
#include <QString>
#include "include/histogramdialog.h"

#include <functional>

template <typename T, typename FUNC>
void doForAllPixels(cv::Mat_<T> &M, const FUNC &func)
{
    int nRows = M.rows;
    int nCols = M.cols;
    for(int i=0; i<nRows; ++i)
    {
        auto *pRow = (T *)M.ptr(i);
        for(int j=0; j<nCols; ++j, ++pRow)
            *pRow = func(*pRow);
    }
}

class Util
{
public:
    static cv::Mat toCv(const QImage &image, int cv_type);
    static QImage toQt(const cv::Mat &src, QImage::Format format);
    static QString cleanNumberAndPunctuation(QString toClean);
    static void plot(const cv::Mat &hist,QWidget *parent);
    static QString fileNameWithoutPath(QString& filePath);
};

#endif
