#ifndef NODE_H
#define NODE_H

#include "precompiled.h"
#include "memory"
#include "PixelCloud.h"

#include <3rdparty/cereal/access.hpp>
#include "Feature.h"

struct Node
{
    quint32 id{};
    quint32 start{};
    quint32 end{};
    quint32 leftCount{};
    qint16 tau{};
    cv::Point teta1{}, teta2{};

    cv::Mat_<float> hist;
    quint8 ftrID{};

    Node() : Node(0)
    {
    }

    Node(quint32 nodeId) : id(nodeId)
    {
        hist.create(1, 1);
        hist.setTo(0);
    }

    ~Node()
    {
        hist.release();
    }

private:
    friend class cereal::access;

    template<class Archive>
    void save(Archive &archive) const
    {
        // cv::Point is not serializable
        const int x1 = teta1.x;
        const int y1 = teta1.y;
        const int x2 = teta2.x;
        const int y2 = teta2.y;
        archive( id , start, end, leftCount, tau, ftrID, hist, x1, y1, x2, y2);

    }

    template<class Archive>
    void load(Archive &archive)
    {
        int x1, y1, x2, y2;
        archive( id , start, end, leftCount, tau, ftrID, hist, x1, y1, x2, y2);
        teta1.x = x1;
        teta1.y = y1;
        teta2.x = x2;
        teta2.y = y2;
//        std::cout << hist << std::endl;
    }
};

struct Node2b : public Node
{
    bool inline isLeft(Pixel &p, const cv::Mat &img)
    {
//        qDebug() << "Node2b";
        auto feature = Feature::features[ftrID];
        auto row = feature.rows;
        auto col = feature.cols;

        auto fr1_x = teta1.x + p.position.x;
        auto fr1_y = teta1.y + p.position.y;
        auto fr2_x = teta2.x + p.position.x;
        auto fr2_y = teta2.y + p.position.y;
        featureType fr1{};
        featureType fr2{};

        const uchar* imgRow1;
        const uchar* imgRow2;
        featureType* ftrRow;
        for (int i = 0; i < row; ++i)
        {
            imgRow1 = img.ptr<uchar>(i + fr1_y);
            imgRow2 = img.ptr<uchar>(i + fr2_y);
            ftrRow = feature.ptr<featureType>(i);
            for (int j = 0; j < col; ++j)
            {
                fr1 += ftrRow[j]*imgRow1[j + fr1_x];
                fr2 += ftrRow[j]*imgRow2[j + fr2_x];
            }
        }

        if(ftrID > 0)
            return fr1 <= tau;
        return (fr1 - fr2) <= tau;
    }
};

struct Node3b : public Node
{ 
    bool inline isLeft(Pixel &p, const cv::Mat &img)
    {
//        qDebug() << "Node3b";
        auto feature = Feature::features[ftrID];
        auto row = feature.rows;
        auto col = feature.cols;

        auto fr1_x = teta1.x + p.position.x;
        auto fr1_y = teta1.y + p.position.y;
        auto fr2_x = teta2.x + p.position.x;
        auto fr2_y = teta2.y + p.position.y;

        cv::Vec3b fr1{};
        cv::Vec3b fr2{};

        for (int i = 0; i < row; ++i)
            for (int j = 0; j < col; ++j)
            {
                fr1 += feature(i,j)*img.at<cv::Vec3b>(fr1_y + i,fr1_x + j);
                fr2 += feature(i,j)*img.at<cv::Vec3b>(fr2_y + i,fr2_x + j);
            }


        if(ftrID > 0)
            fr2 = cv::Vec3b(0,0,0);
        auto dist = cv::norm(fr1,fr2,CV_L2);
        return dist <= tau;
    }
};


#endif // NODE_H
