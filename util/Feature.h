#ifndef FEATURE_H
#define FEATURE_H

#include "precompiled.h"

#define FEATURE_COL_SIZE 7

#define REGION_COUNT 0
#define POS_NEG_REG 1
#define REGION_RATIO 2
#define LEFT 3
#define TOP 4
#define RIGHT 5
#define BOTTOM 6

class Feature
{
public:
    static int max_w;
    static int max_h;
    Feature();
    static QVector<MatFeature> features;
    static QVector<QString> features_str;
    static QVector<MatFeature> features_integral;
    static MatFeature features_mat;
    static void init();
    static void load(QString features_file);
    static QString getVisual(const MatFeature feature);
    static QString getSeperator(const int width);
};

#endif // FEATURE_H
