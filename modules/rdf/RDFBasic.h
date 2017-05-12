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

    cv::Mat inline computeLabels(const cv::Mat &roi)
    {
        cv::Mat_<float> confs;
        cv::Mat_<float> layered = getLayeredHist(roi);
        cv::Mat res;
        getLabelAndConfMat(layered, res, confs);
        return res;
    }


    cv::Mat_<float> getCumulativeProbHist(const cv::Mat_<float> &layeredHist);

    void inline getLabelAndConfMat(cv::Mat_<float> &layeredHist, cv::Mat &labels, cv::Mat_<float> &confs)
    {
        auto labelCount = m_params.labelCount;
        auto nRows = layeredHist.rows;
        auto nCols = layeredHist.cols / labelCount;

        labels = cv::Mat::zeros(nRows, nCols, CV_8UC1);
        confs = cv::Mat_<float>::zeros(nRows, nCols);

        for (int row = 0; row < nRows; ++row)
            tbb::parallel_for(0, nCols, 1, [ =, &labels, &confs ](int col)
            {
                double max;
                cv::Point max_loc;
                auto tmpProbHist = layeredHist(cv::Range(row, row + 1), cv::Range(col * labelCount, (col + 1) * labelCount));
                cv::minMaxLoc(tmpProbHist, NULL, &max, NULL, &max_loc);

                if( max_loc.x == 0)
                    labels.at<uchar>(row, col) = max*255;
                confs(row, col) = max;
            });

//            for (int col = 0; col < nCols; ++col)
//            {
//                auto tmpProbHist = layeredHist(cv::Range(row, row + 1), cv::Range(col * labelCount, (col + 1) * labelCount));
//                cv::minMaxLoc(tmpProbHist, NULL, &max, NULL, &max_loc);
//                labels.at<cv::Vec3b>(row, col) = colorcode.colors[max_loc.x];
//                confs(row, col) = max;
//            }
    }

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
