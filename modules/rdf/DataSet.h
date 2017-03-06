#ifndef DATASET_H
#define DATASET_H

#include "precompiled.h"

struct DataSet
{
    std::vector<cv::Mat> images{};
    std::vector<int> labels{};
    bool isProcessed{};
    bool isBordered{};

    ~DataSet()
    {
        images.clear();
        labels.clear();
    }


};

#endif // DATASET_H
