#ifndef CPV_RANDOM_DECISION_FOREST
#define CPV_RANDOM_DECISION_FOREST

#include "RandomDecisionTree.h"
#include "Util.h"
#include "ocr/TextRegionDetector.h"
#include <3rdparty/cereal/access.hpp>

class RandomDecisionForest : public QObject
{
    Q_OBJECT

public:
    std::vector<rdt_ptr> m_forest;
    RDFParams m_params;
    DataSet m_DS;

private:
    QString m_dir{};
    int m_numOfLetters{};

public:
    void trainForest();
    void setParams(const RDFParams &params) { m_params = params; }
    RDFParams &params() { return m_params; }

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
    void classifiedImageAs(int image_no, char label);
    void resultPercentage(double accuracy);

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
