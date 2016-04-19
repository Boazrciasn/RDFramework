#ifndef CPV_TRDETECTOR
#define CPV_TRDETECTOR

#include <opencv2/core.hpp>
#include <QDialog>
#include "include/Util.h"

class TextRegionDetector{
public:
    static QVector<QRect> detectRegions(const cv::Mat &img_bw, QWidget *parent);
    static QVector<QRect> detectWordsFromLine(const cv::Mat &lineImg, QWidget *parent);
private:
    static QVector<int> extractCoordinateFrom(QVector<int> y);
    static void getRange(const cv::Mat &img_bw, int &leftMargin, int &rightMargin, QWidget *parent);
    static void extractROI(cv::Mat &regHist, int &leftMargin, int &rightMargin);
};

#endif
