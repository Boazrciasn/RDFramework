#ifndef PIXELCLOUD_H
#define PIXELCLOUD_H

#include <memory>
#include "precompiled.h"
#include <tbb/tbb.h>

struct Colorcode
{
    QVector<cv::Vec3b> colors{};

    Colorcode()
    {
        colors.append(cv::Vec3b(0,0,255));
        colors.append(cv::Vec3b(20,20,51));
        colors.append(cv::Vec3b(41,102,66));
        colors.append(cv::Vec3b(51,0,38));
        colors.append(cv::Vec3b(0,0,204));
        colors.append(cv::Vec3b(0,255,106));
        colors.append(cv::Vec3b(255,213,0));
        colors.append(cv::Vec3b(230,92,195));
        colors.append(cv::Vec3b(51,43,0));
        colors.append(cv::Vec3b(0,51,21));
        colors.append(cv::Vec3b(102,41,87));
        colors.append(cv::Vec3b(92,92,229));
        colors.append(cv::Vec3b(82,204,133));
        colors.append(cv::Vec3b(128,115,51));
    }

    int colorCount() { return colors.size(); }
};


struct Pixel
{
    cv::Point position;
    quint32 id;
    int label;

    Pixel() : Pixel(cv::Point(0,0),0,0)
    {
    }

    Pixel(cv::Point pt, quint32 sampleId, int sampleLabel): position(pt),
        id(sampleId), label(sampleLabel)
    {
    }

    Pixel(const Pixel &px): position(px.position), id(px.id), label(px.label)
    {
    }
};

struct PixelCloud
{
    QVector<Pixel>  pixels1{};
    QVector<Pixel>  pixels2{};

    void inline swap()
    {
        auto tmp = pixels2;
        pixels2 = pixels1;
        pixels1 = tmp;
    }
};

#endif // PIXELCLOUD_H
