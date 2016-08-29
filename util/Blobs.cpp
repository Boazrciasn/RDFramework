#include "precompiled.h"
#include "util.h"


// TODO : update this
#define COUNTFOR_BACKGROUND 0

#include "Blobs.h"

void renderBlobs(
    const BinaryImage &imgBinary,
    cv::Mat &imgOutput,
    const IntMat &labels,
    const IntMat &statsv,
    const DoubleMat &centroids,
    const QVector<bool> &bAllowed
)
{
    int nLabels = statsv.rows;
    auto ot = imgOutput.type();
    if (ot == CV_8UC1) // mask output
    {
        doForAllPixels(imgBinary, [&](quint16 value, quint16 x, quint16 y)
        {
            imgOutput.at<quint8>(y, x) = (quint8)(bAllowed[labels(y, x)] ? value : 0);
        });
    }
    else if (ot == CV_16UC1) // label output
    {
        doForAllPixels(imgBinary, [&](quint16 value, quint16 x, quint16 y)
        {
            auto label = labels(y, x);
            imgOutput.at<quint16>(y, x) = bAllowed[label] ? label : 0;
        });
    }
    else if (ot == CV_8UC3) // color output
    {
        std::vector<cv::Vec3b> palette(nLabels);
        quint32 nColors = 0;
        for (int i = COUNTFOR_BACKGROUND; i < nLabels; ++i)
        {
            if (bAllowed[i])
            {
                QColor color = QColor((++nColors * 77) % 360, 0.5, 1.0);
                palette[i] = cv::Vec3b(color.blue(), color.green(), color.red());
            }
        }
        doForAllPixels(imgBinary, [&](quint16 value, quint16 x, quint16 y)
        {
            auto label = labels(y, x);
            imgOutput.at<cv::Vec3b>(y, x) = bAllowed[label] ? palette[label] : cv::Vec3b(0, 0, 0);
        });
    }
}

void doForAllBlobs(const BinaryImage &imgBinary, cv::Mat imgOutput, std::function < bool(BlobData &&) > func)
{
    IntMat labels, statsv;
    DoubleMat centroids;
    cv::connectedComponentsWithStats(imgBinary, labels, statsv, centroids);
    int nLabels = statsv.rows;
    QVector<bool> bAllowed; // array to track if a blob is allowed after the filtering
    if (!imgOutput.empty())
    {
        Q_ASSERT(imgOutput.type() == CV_8UC1 || imgOutput.type() == CV_16UC1 || imgOutput.type() == CV_8UC3);
        bAllowed.resize(nLabels);
    }
    for (int label = COUNTFOR_BACKGROUND; label < nLabels; ++label) // start from 1, 0 is background label
    {
        auto *P = statsv.ptr<int>(label);
        cv::Rect rect{P[cv::CC_STAT_LEFT], P[cv::CC_STAT_TOP], P[cv::CC_STAT_WIDTH], P[cv::CC_STAT_HEIGHT]};
        bool bFilterResult = func(BlobData(rect, P[cv::CC_STAT_AREA], {centroids(label, 0), centroids(label, 1)}));
        if (!bAllowed.empty())
            bAllowed[label] = bFilterResult;
    }
    if (!bAllowed.empty())
        renderBlobs(imgBinary, imgOutput, labels, statsv, centroids, bAllowed);
}
