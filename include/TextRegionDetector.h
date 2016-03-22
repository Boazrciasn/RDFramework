#ifndef CPV_TRDETECTOR
#define CPV_TRDETECTOR

#include <opencv2/core.hpp>
#include <QDialog>
#include "include/Util.h"

class TextRegionDetector{
public:
    static QVector<QRect> detectRegions(const cv::Mat &img_bw, QWidget *parent);

private:
    static QVector<double> extractCoordinateFrom(QVector<double> y);
};

#endif
