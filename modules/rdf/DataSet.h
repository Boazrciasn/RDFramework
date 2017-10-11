#ifndef DATASET_H
#define DATASET_H

#include "precompiled.h"
#include <map>

struct DataSet
{
    QVector<QVector<QRect>> frameRects{};
    std::vector<cv::Mat> images{};
    std::vector<cv::Mat> integral_images{};
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

    QString toString() const
    {
        auto res = "<<<< DataSet >>>>\n   images: "_qs + QString::number(images.size())
                + QString("\n   Images per Label: \n");
        for (auto it = std::begin(map_dataPerLabel); it != std::end(map_dataPerLabel); ++it)
            res += QString("      ") + QString::number(it->first)
                    + QString(" => ") + QString::number(it->second) + QString(" \n");

        return res;
    }
};

#endif // DATASET_H
