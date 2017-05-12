#ifndef RECTANGLEPARTICLE_H
#define RECTANGLEPARTICLE_H

#include "precompiled.h"

struct RectangleParticle
{
    uint16_t x{};
    uint16_t y{};
    uint16_t width{};
    uint16_t height{};
    uint16_t r{};
    int16_t theta{};
    uint16_t age{};
    uint8_t cluster{};
    float weight{};
    bool isVisited{};

    RectangleParticle() {}

    RectangleParticle(quint16 x, quint16 y, quint16 width, quint16 height, float weight) :
        x(x), y(y), width(width), height(height), weight(weight) { }

    RectangleParticle clone()
    {
        RectangleParticle clone(x, y, width, height, weight);
        clone.r = r;
        clone.theta = theta;
        clone.age = age;
        return clone;
    }

    void exec(const cv::Mat& integralImg)
    {
        float weight_outer = integralImg.at<int>(y, x) + integralImg.at<int>(y + height, x + width) -
                integralImg.at<int>(y + height, x) - integralImg.at<int>(y,x + width);

        auto pad = 3;
        float weight_inner = integralImg.at<int>(y + pad, x + pad) + integralImg.at<int>(y + height - pad, x + width - pad) -
                integralImg.at<int>(y + height - pad, x + pad) - integralImg.at<int>(y + pad, x + width - pad);

        weight_outer /= (width*height);
        weight_outer = (weight_outer==0) ? 1:weight_outer;
        weight_inner /= (width*height);

        weight = (weight_inner*weight_inner)/weight_outer;
    }

    inline cv::Rect boundingBox(){
        return cv::Rect(x, y, width, height);
    }
};

#endif // RECTANGLEPARTICLE_H
