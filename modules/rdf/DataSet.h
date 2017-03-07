#ifndef DATASET_H
#define DATASET_H

#include "precompiled.h"
#include <map>

struct DataSet
{
    std::vector<cv::Mat> images{};
    std::vector<int> labels{};
    std::map<quint16,quint16> map_dataPerLabel;
    QString info;
    bool isProcessed{};
    bool isBordered{};

    ~DataSet()
    {
        images.clear();
        labels.clear();
    }

    QString toString()
    {
        QString res = QString("<<<< DataSet >>>>\n   images: ") + QString::number(images.size())
                + QString("\n   Images per Label: \n");
        for (auto it = map_dataPerLabel.begin(); it != map_dataPerLabel.end(); ++it)
            res += QString("      ") + QString::number(it->first)
                    + QString(" => ") + QString::number(it->second) + QString(" \n");

        return res;
    }
};

#endif // DATASET_H
