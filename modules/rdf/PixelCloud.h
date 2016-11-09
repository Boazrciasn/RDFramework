#ifndef PIXELCLOUD_H
#define PIXELCLOUD_H

#include <memory>
#include "precompiled.h"

struct Coord
{
    quint16 m_dx{};
    quint16 m_dy{};

    Coord()
    {
    }

    Coord(quint16 x, quint16 y) : m_dx(x), m_dy(y)
    {
    }

    Coord(const Coord &c) : m_dx(c.m_dx), m_dy(c.m_dy)
    {
    }
};

struct Pixel
{
    Coord position;
    quint8 intensity;
    quint32 sampleId;
    QString sampleLabel;

    Pixel(Coord pt, quint8 intnsty, quint32 id, QString label): position(pt),
        intensity(intnsty), sampleId(id), sampleLabel(label)
    {
    }

    Pixel(const Pixel &px): position(px.position), intensity(px.intensity),
        sampleId(px.sampleId), sampleLabel(px.sampleLabel)
    {
    }
};

struct PixelCloud
{
    QVector<Pixel *> pixels{};

    void swap(int indx1, int indx2)
    {
        Pixel *tmp = pixels.at(indx1);
        pixels.replace(indx1,pixels.at(indx2));
        pixels.replace(indx2,tmp);
    }
};

using pixel_ptr = std::shared_ptr<Pixel>;
//using PixelCloud = std::vector<pixel_ptr>;

#endif // PIXELCLOUD_H
