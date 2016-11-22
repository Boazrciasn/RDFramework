#ifndef CPV_RANDOM_DECISION_FOREST
#define CPV_RANDOM_DECISION_FOREST

#include "RandomDecisionTree.h"
#include <3rdparty/cereal/access.hpp>

class RandomDecisionForest : public QObject
{
    Q_OBJECT

private:
    std::vector<rdt_ptr> m_forest;
    RDFParams m_params{};
    DataSet m_DS{};

public:
    bool trainForest();
    void detect(cv::Mat &roi, int &label, float &conf);
    cv::Mat_<float> getLayeredHist(cv::Mat &roi);
    void getCumulativeProbHist(cv::Mat_<float> &probHist, const cv::Mat_<float> &layeredHist);
    void getLabelAndConfMat(cv::Mat_<float> &layeredHist,
                            cv::Mat_<uchar> &labels, cv::Mat_<float> &confs);

    void setParams(const RDFParams &params) { m_params = params; }
    void setDataSet(const DataSet &DS)
    {
        m_DS = DS;
        preprocessDS();
    }

    inline void preprocessDS(){
        for(auto &img : m_DS.m_ImagesVector)
            img = 255 - img;
    }

    RDFParams &params() { return m_params; }
    DataSet &DS() { return m_DS; }

    inline void printForest()
    {
        qDebug() << "FOREST {";
        for(rdt_ptr tree : m_forest)
            tree->printTree();
        qDebug() << "}";
    }

    RandomDecisionForest() { srand(time(nullptr)); }
    ~RandomDecisionForest() { m_forest.clear(); }

signals:
    void treeConstructed();

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
    void serialize( Archive & archive )
    {
        archive(m_params);
        for(auto rdt : m_forest)
            archive(*rdt);
    }

};
#endif
