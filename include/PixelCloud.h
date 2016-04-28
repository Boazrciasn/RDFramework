#ifndef PIXELCLOUD_H
#define PIXELCLOUD_H

#include <memory>

struct Coord
{
    int m_dx;
    int m_dy;

    Coord() : Coord(0, 0)
    {
    }

    Coord(int x, int y) : m_dx(x), m_dy(y)
    {
    }

    Coord(const Coord& c) : m_dx(c.m_dx), m_dy(c.m_dy)
    {
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive( m_dx, m_dy ); // serialize things by passing them to the archive
    }
};

struct ImageInfo
{
    QString label;
    int sampleId;

    ImageInfo() : ImageInfo("", 0)
    {
    }

    ImageInfo(const QString &lbl, int smplId) : label(lbl), sampleId(smplId)
    {
    }
};

using imageinfo_ptr = std::shared_ptr<ImageInfo>;

struct Pixel
{
    Coord position;
    quint8 intensity;
    imageinfo_ptr imgInfo;

    Pixel(Coord crd, unsigned char intnsty, imageinfo_ptr imgInf): position(crd), intensity(intnsty),imgInfo(imgInf)
    {
    }
};

using pixel_ptr = std::shared_ptr<Pixel>;
using PixelCloud = std::vector<pixel_ptr>;

#endif // PIXELCLOUD_H
