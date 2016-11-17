#ifndef DATASET_H
#define DATASET_H

#include "precompiled.h"

struct DataSet
{
    std::vector<cv::Mat> m_ImagesVector;
    std::vector<int> m_labels;

    ~DataSet()
    {
        m_ImagesVector.clear();
        m_labels.clear();
    }
};

#endif // DATASET_H
