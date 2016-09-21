#ifndef BLOBS_H
#define BLOBS_H


#include "typedefs.h"

class BlobData
{
    cv::Rect    m_rect;
    quint32     m_area;
    cv::Vec2d   m_centroids;

public:
    BlobData(const cv::Rect &rect, quint32 area, const cv::Vec2d &centroids) :
        m_rect(rect), m_area(area), m_centroids(centroids) { }

    inline const cv::Rect &rect() const { return m_rect; }
    inline int width() const { return m_rect.width; }
    inline int height() const { return m_rect.height; }
    inline quint32 area() const { return m_area; }
    inline const cv::Vec2d &centroids() const { return m_centroids; }
//    inline float elongation() const { return Frodo::bigRatio(m_rect.width, m_rect.height); }
    inline float fullness() const { return (float)m_area / (m_rect.width * m_rect.height); }
};

void renderBlobs(
        const BinaryImage &imgBinary,
        cv::Mat &imgOutput,
        const IntMat &labels,
        const IntMat &statsv,
        const DoubleMat &centroids,
        const QVector<bool> &bAllowed
        );

void doForAllBlobs(const BinaryImage &imgBinary, cv::Mat imgOutput, std::function<bool(BlobData &&)> func);

inline void doForAllBlobs(const BinaryImage &imgBinary, std::function<bool(BlobData &&BD)> func)
{
    doForAllBlobs(imgBinary, cv::Mat(), func);
}

#endif // BLOBS_H
