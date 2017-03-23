#ifndef CPV_RANDOM_DECISION_FOREST
#define CPV_RANDOM_DECISION_FOREST

#include "RandomDecisionTree.h"
#include "util/SignalSenderInterface.h"
#include "StatisticsLogger.h"
#include "PreProcess.h"


#include <3rdparty/cereal/access.hpp>

class RandomDecisionForest
{

  private:
    std::vector<RandomDecisionTree> m_trees{};
    RDFParams m_params{};
    DataSet *m_DS;
    std::map<quint16,quint16> m_map_pxPerLabel;
    quint32 m_nTreesForDetection;
    Colorcode colorcode;
    StatisticsLogger m_statLog;
    PreProcess m_preProcess;

public:
    bool trainForest();
    float testForest();
    float testForest(tbb::concurrent_vector<cv::Mat>& output);
    void detect(cv::Mat &roi, int &label, float &conf);
    cv::Mat_<float> getLayeredHist(cv::Mat &roi);
    void getCumulativeProbHist(cv::Mat_<float> &probHist, const cv::Mat_<float> &layeredHist);
    void getLabelAndConfMat(cv::Mat_<float> &layeredHist,
                            cv::Mat &labels, cv::Mat_<float> &confs);

    void setParams(const RDFParams &params)
    {
        m_params = params;
        m_trees.resize(m_params.nTrees);
        m_nTreesForDetection = m_params.nTrees;
    }

    inline void addBorder()
    {
        std::vector<Process*> preprocess{new MakeBorder(m_params.probDistX + Feature::max_w, m_params.probDistY + Feature::max_h, cv::BORDER_CONSTANT)};
        PreProcess::doBatchPreProcess(m_DS->images, preprocess);
    }

    inline void computePixelPerImgMap()
    {
        auto maxLabel = std::max_element(m_DS->map_dataPerLabel.begin(), m_DS->map_dataPerLabel.end(),
            [](const std::pair<quint16,quint16>& p1, const std::pair<quint16,quint16>& p2) {
                return p1.second < p2.second; });
        float maxTotPerLabel = maxLabel->second * m_params.pixelsPerImage;

        for (auto it = m_DS->map_dataPerLabel.begin(); it != m_DS->map_dataPerLabel.end(); ++it)
            m_params.pixelsPerLabelImage[it->first] = std::round(maxTotPerLabel/it->second);

//        for (auto it = m_DS->map_dataPerLabel.begin(); it != m_DS->map_dataPerLabel.end(); ++it)
//            qDebug() << it->second << " * " << m_params.pixelsPerLabelImage[it->first] << " = " <<
//                        it->second*m_params.pixelsPerLabelImage[it->first];
    }

    inline void setDataSet(DataSet* DS)
    {
        m_DS = DS;
        if (!m_DS->isBordered)
        {
            addBorder();
            m_DS->isBordered = true;
        }
        computePixelPerImgMap();
    }

    inline void preprocessDS()
    {
        m_preProcess.doBatchPreProcess(m_DS->images);
        m_DS->isProcessed = true;
    }

    inline void setPreProcess(std::vector<Process*> preprocess)
    {
        preprocess.push_back(new MakeBorder(m_params.probDistX, m_params.probDistY, cv::BORDER_CONSTANT));
        m_preProcess.setPreProcess(preprocess);
    }

    inline auto params()       -> RDFParams &
    { return m_params; }

    inline auto DS()           -> DataSet *
    { return m_DS; }

    void inline setNTreesForDetection(quint32 count)
    {
        if (count > m_trees.size())
            return;
        m_nTreesForDetection = count;
    }

    RandomDecisionForest()
    {
        srand(time(nullptr));
        m_nTreesForDetection = 1;
        Feature::init();
        TableLookUp::init();
    }
    SignalSenderInterface m_signalInterface;
    ~RandomDecisionForest() { m_trees.clear(); }

    // Cereal serialize code:
  public:
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
    friend class cereal::access;

    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(m_params, m_trees);
        for (auto &rdt : m_trees)
            rdt.setParams(&m_params);
    }

    void printDataInfo();
};
#endif
