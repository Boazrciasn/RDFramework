#ifndef CPV_RANDOM_DECISION_FOREST
#define CPV_RANDOM_DECISION_FOREST

#include "include/RandomDecisionTree.h"
#include "include/Util.h"
#include "include/TextRegionDetector.h"

class RandomDecisionForest : public QObject
{
    Q_OBJECT

    friend class RandomDecisionTree;

  public:
    RandomDecisionForest()
    {
        srand(time(NULL));
        //       m_begin = rdfclock::now();
    }

    ~RandomDecisionForest()
    {
        //m_forest.clear();
    }

    //    template<class Archive>
    //    void serialize(Archive & archive)
    //    {
    //        archive(m_params);
    //        for(rdt_ptr rdt : m_forest)
    //        {
    //            archive(rdt);
    //        }
    //    }


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

    void readAndIdentifyWords();
    void searchWords(QString query, int queryId);
    void readTrainingImageFiles();
    void readTestImageFiles();
    void printPixelCloud();
    void printPixel(pixel_ptr px);
    int pixelCloudSize();
    cv::Mat getPixelImage(pixel_ptr px);
    void imageToPixels(std::vector<pixel_ptr> &res, const cv::Mat &image,
                       imageinfo_ptr img_inf);
    cv::Mat colorCoder(const cv::Mat &labelImage, const cv::Mat &InputImage);
    void trainForest();
    void test();
    cv::Mat getLayeredHist(cv::Mat test_image, int index,
                           QVector<quint32> &fgPxNumberPerCol);
    RDFParams &params()
    {
        return m_params;
    }
    void setParams(const RDFParams &params)
    {
        m_params = params;
    }
    DataSet m_DS;
    std::vector<rdt_ptr> m_forest;
    // Keep all images on memory
    std::vector<cv::Mat> m_imagesContainer;
    inline void setParentWidget(QWidget *parent_widget)
    {
        m_parent = parent_widget;
    }

    inline void printForest()
    {
        qDebug() << "FOREST {";
        for(rdt_ptr tree : m_forest)
            tree->printTree();
        qDebug() << "}";
    }

    QWidget *m_parent;
    RDFParams m_params;

  private:
    //    rdfclock::time_point m_begin;

    void placeHistogram(cv::Mat &output, const cv::Mat &pixelHist, int pos_row,
                        int pos_col);
    cv::Mat_<float> createLetterConfidenceMatrix(const cv::Mat &layeredHist, const QVector<quint32> &fgPxNumberPerCol);
    double m_accuracy;
    std::vector<QString> classify_res;

    QString m_dir;
    int m_numOfLetters = 0;

  signals:
    void classifiedImageAs(int image_no, char label);
    void treeConstructed();
    void resultPercentage(double accuracy);

};

template<class Archive>
void save(Archive &archive,
          RandomDecisionForest const &rdf)
{
    archive( rdf.m_params );
    for(auto rdtPtr : rdf.m_forest)
    {
        archive(*rdtPtr);
    }
}

template<class Archive>
void load(Archive &archive,
          RandomDecisionForest &rdf)
{
    archive( rdf.m_params );
    auto size = rdf.m_params.nTrees;
    rdf.m_forest.resize(size);
    for(auto i = 0; i < size; ++i)
    {
        rdt_ptr rdt(new RandomDecisionTree(&rdf));
        archive(*rdt);
        rdf.m_forest[i] = rdt;
    }
}

#endif
