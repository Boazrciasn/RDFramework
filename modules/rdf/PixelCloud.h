#ifndef PIXELCLOUD_H
#define PIXELCLOUD_H

#include <memory>
#include "precompiled.h"
#include <tbb/tbb.h>
//#include <vector>

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
    tbb::concurrent_vector<Pixel *> pixels{};
//    std::vector<Pixel *> pixels{};

    void inline swap(int indx1, int indx2)
    {
        Pixel *tmp = pixels.at(indx1);
        pixels[indx1] = pixels.at(indx2);
        pixels[indx2] = tmp;
    }

    // L: false  R: true
    void putInOrder(int start, QVector<bool> relation)
    {
        int count = relation.size();
        bool terminate = false;
        for(int i = 0; i < count-1; ++i)
        {
            if(relation[i]) // R detected
                for(int j = i+1; j < count; ++j)
                    if(!relation[j]) // L detected
                    {
                        swap(start+i,start+j);
                        relation[j] = true;
                        terminate = (j == count-1); // no need to search
                        break;
                    }
            if(terminate)
                break;
        }
    }
};

#endif // PIXELCLOUD_H
