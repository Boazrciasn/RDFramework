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
    DataSet m_DS{};
    quint32 m_nTreesForDetection;
    Colorcode colorcode;
    StatisticsLogger m_statLog;

  public:
    bool trainForest();
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
    void setDataSet(const DataSet &DS)
    {
        m_DS = DS;
        preprocessDS();
    }

    inline void preprocessDS()
    {
        InverseImage invImg;
        MakeBorder mBorder(m_params.probDistX, m_params.probDistY, cv::BORDER_CONSTANT);
        Gaussian gg(3,3,0);
        invImg.process(m_DS.m_ImagesVector);
        gg.process(m_DS.m_ImagesVector);
        mBorder.process(m_DS.m_ImagesVector);
    }

    RDFParams &params() { return m_params; }
    DataSet &DS() { return m_DS; }

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

};
#endif
