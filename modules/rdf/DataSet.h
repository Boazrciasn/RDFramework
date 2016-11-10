#ifndef DATASET_H
#define DATASET_H

#include "precompiled.h"

struct DataSet
{

    QHash<int, cv::Mat> m_TrainHashTable;
    std::vector<cv::Mat> m_trainImagesVector;
    std::vector<cv::Mat> m_testImagesVector;
    std::vector<QString> m_testlabels;
    std::vector<QString> m_trainlabels;

    ~DataSet()
    {
        m_TrainHashTable.clear();
        m_trainImagesVector.clear();
        m_testImagesVector.clear();
        m_testlabels.clear();
        m_trainlabels.clear();
    }
};

#endif // DATASET_H
