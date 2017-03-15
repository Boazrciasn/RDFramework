#ifndef FEATURE_H
#define FEATURE_H

#include "precompiled.h"

class Feature
{
public:
    static int max_w;
    static int max_h;
    Feature();
    static std::vector<MatFeature> features;
};

#endif // FEATURE_H
