#ifndef RDFBASIC_H
#define RDFBASIC_H
#include "RDTBasic.h"
#include "RDFParams.h"
#include "PixelCloud.h"

class RDFBasic
{
public:
    RDFBasic() { }
    ~RDFBasic() {
        m_trees.clear();
        delete &m_params;
        delete &colorcode;
    }

    void inline detect(const cv::Mat& roi, int& label, float &conf)
    {
        auto probHist = getCumulativeProbHist(getLayeredHist(roi));
        auto max = 0.0;
        cv::Point max_loc;
        cv::minMaxLoc(probHist, NULL, &max, NULL, &max_loc);
        label = max_loc.x;
        conf = max;
    }

    int inline getLabel(const cv::Mat& roi)
    {
        auto label = 0;
        auto dummy = 0.0f;
        detect(roi, label, dummy);
        return label;
    }

    float inline getDetectionConfidence(const cv::Mat& roi)
    {
        auto dummy = 0;
        auto conf  = 0.0f;
        detect(roi, dummy, conf);
        return conf;
    }

    cv::Mat_<float> inline getLayeredHist(const cv::Mat &roi)
    {
        cv::Mat_<float> layeredHist = cv::Mat_<float>::zeros(roi.rows, roi.cols * m_params.labelCount);
        for(auto i = 0; i < m_nTreesForDetection; ++i)
            layeredHist += m_trees[i].getLayeredHist(roi);
        return layeredHist/m_nTreesForDetection;
    }

    cv::Mat_<float> getCumulativeProbHist(const cv::Mat_<float> &layeredHist);
    void getLabelAndConfMat(cv::Mat_<float> &layeredHist, cv::Mat &labels, cv::Mat_<float> &confs);
    int inline getTreeCount() { return m_trees.size(); }

    void inline setNTreesForDetection(quint32 count)
    {
        if (count <= m_trees.size())
            m_nTreesForDetection = count;
    }


    //////////////////////////////
    /// Cereal serialize code: ///
    //////////////////////////////

    inline void saveForest(QString fname)
    {
        std::ofstream file(fname.toStdString(), std::ios::binary);
        cereal::BinaryOutputArchive ar(file);
        ar(*this);
        file.flush();
        file.close();
        file.close();
    }

    inline void loadForest(QString fname)
    {
        std::ifstream file(fname.toStdString(), std::ios::binary);
        cereal::BinaryInputArchive ar(file);
        ar(*this);
        file.close();
    }

private:
    std::vector<RDTBasic> m_trees{};
    quint8 m_nTreesForDetection;
    RDFParams m_params;
    Colorcode colorcode;

    friend class cereal::access;

    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(m_params, m_trees);
        setNTreesForDetection(m_trees.size());
    }
};

#endif // RDFBASIC_H
