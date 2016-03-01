#ifndef CPV_UTIL
#define CPV_UTIL

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QImage>
#include <QString>

class Util{
public:
    static cv::Mat toCv(const QImage &image, int cv_type);
    static QImage toQt(const cv::Mat &src, QImage::Format format);
    static QString cleanNumberAndPunctuation(QString toClean);
};

#endif
