#ifndef RDTBASIC_H
#define RDTBASIC_H

#include "precompiled.h"
#include <3rdparty/cereal/archives/binary.hpp>
#include <3rdparty/cereal/types/vector.hpp>
#include <3rdparty/cereal/access.hpp>
#include "3rdparty/matcerealisation.hpp"

#include "Node.h"

extern void computeLayeredHist_gpu(
        const cv::Mat& img,
        cv::Mat_<float>& layered_hist,
        const cv::Mat_<int>& tree_nodes,
        const cv::Mat_<float>& features,
        int lbl_count,
        int padding_x,
        int padding_y
        );

class RDTBasic
{
public:
    RDTBasic() { }
    ~RDTBasic()
    {
        m_nodes.release();
        m_features.release();
    }

    cv::Mat_<float> inline getLayeredHist(const cv::Mat& roi)
    {
        cv::Mat_<float> layeredHist = cv::Mat_<float>::zeros(roi.rows, roi.cols * m_label_count);
//        computeLayeredHist(roi, layeredHist);
        computeLayeredHist_gpu(roi, layeredHist, m_nodes, m_features, m_label_count, m_padding_x, m_padding_y);
        return layeredHist;
    }

private:
    cv::Mat_<qint32> m_nodes{};
    cv::Mat_<featureType> m_features{};
    quint16 m_padding_x{};
    quint16 m_padding_y{};
    quint8 m_label_count{};
    quint8 feature_n{};

    void computeLayeredHist(const cv::Mat& roi, cv::Mat_<float>& layeredHist);
    bool inline isLeaf(qint32 id) { return id == -1;}
    void processPixel(qint16 x, qint16 y, const cv::Mat &roi, cv::Mat_<float> &layeredHist);



    //////////////////////////////
    /// Cereal serialize code: ///
    //////////////////////////////

    friend class cereal::access;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(quint32(), std::vector<Node3b>() , m_nodes, m_features, m_label_count, m_padding_x, m_padding_y);
    }
};

#endif // RDTBASIC_H
