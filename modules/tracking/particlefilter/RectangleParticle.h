#ifndef RECTANGLEPARTICLE_H
#define RECTANGLEPARTICLE_H

#include "precompiled.h"

class RectangleParticle
{
public:
    uint16_t x{};
    uint16_t y{};
    uint16_t width{};
    uint16_t height{};
    uint8_t r{};
    int16_t theta{};
    uint16_t age{};
    uint8_t cluster{};
    float weight{};
    bool isVisited{};


    float dx{};
    float dy{};
    float sig_x{};
    float sig_y{};
    float sig_w{};
    float sig_h{};

    RectangleParticle() {}

    RectangleParticle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, float weight) :
        x(x), y(y), width(width), height(height), weight(weight) { }

    RectangleParticle clone()
    {
        RectangleParticle clone(x, y, width, height, weight);
        clone.r = r;
        clone.theta = theta;
        clone.age = age;
        clone.dx = dx;
        clone.dy = dy;
        clone.sig_x = sig_x;
        clone.sig_y = sig_y;
        return clone;
    }

    void exec(const cv::Mat& integralImg)
    {
        float weight_outer = integralImg.at<int>(y, x) + integralImg.at<int>(y + height, x + width) -
                integralImg.at<int>(y + height, x) - integralImg.at<int>(y,x + width);

        auto pad = 10;
        float weight_inner = integralImg.at<int>(y + pad, x + pad) + integralImg.at<int>(y + height - pad, x + width - pad) -
                integralImg.at<int>(y + height - pad, x + pad) - integralImg.at<int>(y + pad, x + width - pad);

        weight_outer /= (width*height);
        weight_outer = (weight_outer==0) ? 1 : weight_outer;
        weight_inner /= (width*height);

        weight = weight_outer; //(weight_inner*weight_inner)/weight_outer;
    }

    inline cv::Rect boundingBox()
    {
        return cv::Rect(x, y, width, height);
    }

    inline cv::Point center(){
        return cv::Point(x + width/2, y + height/2);
    }
};

#endif // RECTANGLEPARTICLE_H
