#ifndef RECTANGLEPARTICLE_H
#define RECTANGLEPARTICLE_H

#include "precompiled.h"

struct RectangleParticle
{
    quint16 x{};
    quint16 y{};
    quint16 width{};
    quint16 height{};
    qint16 dx{};         // velocity increment x direction
    qint16 dy{};         // velocity increment y direction
    float weight{};
    bool isVisited{};

    RectangleParticle() {}

    RectangleParticle(quint16 x, quint16 y, quint16 width, quint16 height, float weight) :
        x(x), y(y), width(width), height(height), weight(weight) { }

    RectangleParticle clone()
    {
        RectangleParticle clone(x, y, width, height, weight);
        clone.isVisited = isVisited;
        clone.isVisited = dx;
        clone.dy = dy;
        return clone;
    }

    void exec(const cv::Mat& img)
    {
        weight = 0.0f;
        for (int row = y; row < y+height; ++row)
            for (int col = x; col < x + width ; ++col)
                weight +=  img.at<uchar>(row,col);
    }

    inline cv::Rect boundingBox(){
        return cv::Rect(x, y, width, height);
    }
};

#endif // RECTANGLEPARTICLE_H
