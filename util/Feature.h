#ifndef FEATURE_H
#define FEATURE_H

#include "precompiled.h"

class Feature
{
public:
    static int max_w;
    static int max_h;
    Feature();
    static QVector<MatFeature> features;
    static QVector<QString> features_str;
    static void init();
};

#endif // FEATURE_H
