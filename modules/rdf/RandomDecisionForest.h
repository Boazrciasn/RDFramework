#ifndef CPV_RANDOM_DECISION_FOREST
#define CPV_RANDOM_DECISION_FOREST

#include "RandomDecisionTree.h"
#include <3rdparty/cereal/access.hpp>

class RandomDecisionForest : public QObject
{
    Q_OBJECT

private:
    std::vector<RandomDecisionTree> m_trees{};
    RDFParams m_params{};
    DataSet m_DS{};
    quint32 m_nTreesForDetection;
    Colorcode colorcode;

public:
    bool trainForest();
    void detect(cv::Mat &roi, int &label, float &conf);
    cv::Mat_<float> getLayeredHist(cv::Mat &roi);
    void getCumulativeProbHist(cv::Mat_<float> &probHist, const cv::Mat_<float> &layeredHist);
    void getLabelAndConfMat(cv::Mat_<float> &layeredHist,
                            cv::Mat &labels, cv::Mat_<float> &confs);

    void setParams(const RDFParams &params) {
        m_params = params;
        m_trees.resize(m_params.nTrees);
        m_nTreesForDetection = m_params.nTrees;
    }
    void setDataSet(const DataSet &DS)
    {
        m_DS = DS;
        preprocessDS();
    }

    inline void preprocessDS(){
        for(auto &img : m_DS.m_ImagesVector)
        {
            img = 255 - img;  // TODO: uncomment for original images
            cv::GaussianBlur(img,img,cv::Size(5,5),0);
            cv::copyMakeBorder(img,img,m_params.probDistY, m_params.probDistY,
                               m_params.probDistX,m_params.probDistX, cv::BORDER_CONSTANT);
        }
    }

    RDFParams &params() { return m_params; }
    DataSet &DS() { return m_DS; }

    void inline setNTreesForDetection(quint32 count){
        if(count > m_trees.size())
            return;
        m_nTreesForDetection = count;
    }

    RandomDecisionForest() { srand(time(nullptr)); }
    ~RandomDecisionForest() { m_trees.clear(); }

signals:
    void treeConstructed();
    void printTrainMsg(QString);
    void printDetectMsg(QString);

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
        archive(m_params, m_trees);
        for(auto &rdt : m_trees)
            rdt.setParams(&m_params);
    }

};
#endif
