#ifndef PIXELCLOUD_H
#define PIXELCLOUD_H

#include <memory>
#include "precompiled.h"
#include <tbb/tbb.h>

struct Pixel
{
    cv::Point position;
    quint32 id;
    QString label;

    Pixel() : Pixel(cv::Point(0,0),0,"")
    {
    }

    Pixel(cv::Point pt, quint32 sampleId, QString sampleLabel): position(pt),
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
