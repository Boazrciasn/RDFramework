#ifndef PIXELCLOUD_H
#define PIXELCLOUD_H

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
};

struct ImageInfo
{
    QString label;
    int sampleId;
    ImageInfo(const QString &lbl, int smplId) :
        label(lbl), sampleId(smplId){}
};

struct Pixel
{
    Coord position;
    quint8 intensity;
    ImageInfo* imgInfo;
    Pixel(Coord crd, unsigned char intnsty, ImageInfo* imgInf):
        position(crd), intensity(intnsty),imgInfo(imgInf){}
};

using PixelCloud = std::vector<Pixel*>;

#endif // PIXELCLOUD_H
