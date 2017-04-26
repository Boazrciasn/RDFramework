#include "RDTBasic.h"
#include <tbb/parallel_for.h>

void RDTBasic::computeLayeredHist(const cv::Mat& roi, cv::Mat_<float> &layeredHist)
{
    feature_n = (quint8)std::sqrt(m_features.cols);
    int rows = roi.rows;
    int cols = roi.cols;

    cv::Mat padded_roi = roi.clone();
    cv::copyMakeBorder(padded_roi, padded_roi, m_padding_y, m_padding_y, m_padding_x, m_padding_x, cv::BORDER_CONSTANT);

    for (auto x = 0; x < cols; ++x) {
        tbb::parallel_for(0, rows, 1, [ =, &layeredHist ](int y)
        {
            processPixel(x, y, roi, layeredHist);
        });
    }
}

void RDTBasic::processPixel(qint16 x, qint16 y, const cv::Mat &roi, cv::Mat_<float>& layeredHist)
{
    auto xIndex = x + m_padding_x;
    auto yIndex = y + m_padding_y;

    auto* current_node = m_nodes.ptr<qint32>(0);  // root node
    auto  rightID      = current_node[0];
    auto  leftID       = 1;

    while(!isLeaf(rightID))
    {
        auto  ftrID   = current_node[1];
        auto  tau     = current_node[2];
        auto  fr1_x   = xIndex + current_node[3];
        auto  fr1_y   = yIndex + current_node[4];
        auto  fr2_x   = xIndex + current_node[5];
        auto  fr2_y   = yIndex + current_node[6];
        auto* feature = m_features.ptr<float>(ftrID);

        cv::Vec3f ftr1{};
        cv::Vec3f ftr2{};


        if(ftrID != 0)
        {
            auto tot_features = feature_n*feature_n;
            for(int i = 0; i < tot_features; ++i)
                ftr1 += feature[i] * (cv::Vec3f)roi.at<cv::Vec3b>(fr1_y + i / feature_n, fr1_x + i % feature_n);
        }
        else
        {
            ftr1 = roi.at<cv::Vec3b>(fr1_y, fr1_x);
            ftr2 = roi.at<cv::Vec3b>(fr2_y, fr2_x);
        }


        if(cv::norm(ftr1,ftr2,CV_L2) <= tau) {
            current_node = m_nodes.ptr<int>(leftID);
            leftID++;
        }
        else {
            current_node = m_nodes.ptr<int>(rightID);
            leftID = rightID + 1;
        }

        rightID = current_node[0];
    }

    for (int i = 0; i < m_label_count; ++i)
        layeredHist(yIndex - m_padding_y, (xIndex - m_padding_x)*m_label_count + i) = (float)current_node[i+1]/100.0f;
}
