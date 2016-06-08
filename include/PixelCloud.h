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

    Coord(const Coord &c) : m_dx(c.m_dx), m_dy(c.m_dy)
    {
    }

    ~Coord()
    {
    }

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive( m_dx, m_dy ); // serialize things by passing them to the archive
    }
};

struct ImageInfo
{
    QString m_label;
    int m_sampleId;

    ImageInfo() : ImageInfo("", 0)
    {
    }

    ImageInfo(const QString &lbl, int smplId) : m_label(lbl), m_sampleId(smplId)
    {
    }

    ImageInfo(const ImageInfo &ii) : m_label(ii.m_label), m_sampleId(ii.m_sampleId)
    {
    }

    ~ImageInfo()
    {
    }
};

using imageinfo_ptr = std::shared_ptr<ImageInfo>;

struct Pixel
{
    Coord position;
    quint8 intensity;
    imageinfo_ptr imgInfo;

    Pixel(Coord crd, unsigned char intnsty, imageinfo_ptr imgInf): position(crd),
        intensity(intnsty), imgInfo(imgInf)
    {
    }

    Pixel(const Pixel &px): position(px.position), intensity(px.intensity),
        imgInfo(px.imgInfo)
    {
    }

    ~Pixel()
    {
    }
};

using pixel_ptr = std::shared_ptr<Pixel>;
using PixelCloud = std::vector<pixel_ptr>;

#endif // PIXELCLOUD_H
