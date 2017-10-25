#ifndef NODE_H
#define NODE_H

#include "precompiled.h"
#include "memory"
#include "PixelCloud.h"

#include <3rdparty/cereal/access.hpp>
#include <armadillo>
#include "Feature.h"

struct Node
{
    uint32_t id{};
    uint32_t start{};
    uint32_t end{};
    uint32_t leftCount{};

    int32_t dx{};
    int32_t dy{};
    uint16_t hw{};          // half width
    arma::vec vote = arma::vec(4);
    tauType tau{};
    cv::Point teta1{}, teta2{};

    cv::Mat_<float> hist;
    ftrIdType ftrID{};
    bool isLeaf{};

    Node() : Node(0)
    {
    }

    Node(quint32 nodeId) : id(nodeId)
    {
        hist.create(1, 1);
        hist.setTo(0);
        isLeaf = false;
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
        // cv::Point & arma:mat etc. is not serializable
        const int x1 = teta1.x;
        const int y1 = teta1.y;
        const int x2 = teta2.x;
        const int y2 = teta2.y;
        const double cx = vote(0);
        const double cy = vote(1);
        const double w = vote(2);
        const double h = vote(3);
        archive( id , start, end, leftCount, tau, ftrID, hist, isLeaf, x1, y1, x2, y2, dx, dy, hw, cx, cy, w, h);

    }

    template<class Archive>
    void load(Archive &archive)
    {
        int x1, y1, x2, y2;
        double cx, cy, w, h;
        archive( id , start, end, leftCount, tau, ftrID, hist, isLeaf, x1, y1, x2, y2, dx, dy, hw, cx, cy, w, h);
        teta1.x = x1;
        teta1.y = y1;
        teta2.x = x2;
        teta2.y = y2;
        vote = {cx, cy, w, h};
    }
};

struct Node2b : public Node
{
    bool inline isLeft(Pixel &p, const cv::Mat &img)
    {
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
        auto feature = Feature::features[ftrID];
        auto row = feature.rows;
        auto col = feature.cols;

        auto fr1_x = teta1.x + p.position.x;
        auto fr1_y = teta1.y + p.position.y;
        auto fr2_x = teta2.x + p.position.x;
        auto fr2_y = teta2.y + p.position.y;

        cv::Vec3f fr1{};
        cv::Vec3f fr2{};

        for (int i = 0; i < row; ++i)
            for (int j = 0; j < col; ++j)
            {
                fr1 += feature(i,j)*(cv::Vec3f)img.at<cv::Vec3b>(fr1_y + i,fr1_x + j);
                fr2 += feature(i,j)*(cv::Vec3f)img.at<cv::Vec3b>(fr2_y + i,fr2_x + j);
            }

        if(ftrID > 0)
            fr2 = cv::Vec3f(0,0,0);
        auto dist = cv::norm(fr1,fr2,CV_L2);
        return dist <= tau;
    }
};

struct NodeIntegral2b : public Node
{
    bool inline isLeft(Pixel &p, const cv::Mat &integral_img)
    {
        auto feature = Feature::features_integral[ftrID];

        auto fr1_x = teta1.x + p.position.x;
        auto fr1_y = teta1.y + p.position.y;

        auto fr2_x = feature(1,0)*(teta2.x + p.position.x) + (((int)feature(1,0) + 1)%2)*fr1_x;
        auto fr2_y = feature(1,0)*(teta2.y + p.position.y) + (((int)feature(1,0) + 1)%2)*fr1_y;

        featureType ftr{};

        auto reg_count = (int)feature(0, REGION_COUNT);
        for (auto reg = 0; reg < reg_count; ++reg)
        {
            auto switch_1 = (int)(feature(reg, POS_NEG_REG) + 1)/2;
            auto switch_2 = (switch_1 + 1) % 2;
            auto tmp_x = (int)(switch_1*fr1_x + switch_2*fr2_x);
            auto tmp_y = (int)(switch_1*fr1_y + switch_2*fr2_y);

            auto area = (float)(
                    integral_img.at<int>(tmp_y + feature(reg, TOP), tmp_x + feature(reg, LEFT)) -
                    integral_img.at<int>(tmp_y + feature(reg, TOP), tmp_x + feature(reg, RIGHT) + 1) -
                    integral_img.at<int>(tmp_y + feature(reg, BOTTOM) + 1, tmp_x + feature(reg, LEFT)) +
                    integral_img.at<int>(tmp_y + feature(reg, BOTTOM) + 1, tmp_x + feature(reg, RIGHT) + 1));

            ftr += area * feature(reg, POS_NEG_REG) * feature(reg, REGION_RATIO);
        }
        return ftr <= tau;
    }
};


#endif // NODE_H
