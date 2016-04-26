#ifndef CPV_RANDOM_DECISION_FOREST
#define CPV_RANDOM_DECISION_FOREST

#include "include/RandomDesicionTree.h"


class RandomDecisionForest : public QObject
{
    Q_OBJECT

    friend class RandomDecisionTree;

public:
    RandomDecisionForest()
    {
        srand(time(NULL));
    }

    ~RandomDecisionForest()
    {
        for(auto *DT : m_forest)
            delete DT;
        m_forest.clear();
    }

    void readTrainingImageFiles();
    void readTestImageFiles();
    void printPixelCloud();
    void printPixel(Pixel* px);

    int pixelCloudSize();
    cv::Mat getPixelImage(Pixel* px);

    void imageToPixels(std::vector<Pixel*>& res, const cv::Mat &image,ImageInfo* img_inf);
    cv::Mat colorCoder(const cv::Mat &labelImage, const cv::Mat &InputImage);
    void setTrainPath(QString path);
    void setTestPath(QString path);
    void trainForest();

    void test();
    cv::Mat classify(int index);

    RDFParams &params() { return m_params; }
    void setParams(const RDFParams &params) { m_params = params; }

    DataSet m_DS;

    std::vector<RandomDecisionTree *> m_forest;
    int m_no_of_trees;
    // Keep all images on memory
    std::vector<cv::Mat> m_imagesContainer;

    QString m_testpath;
    QString m_trainpath;

private:
    RDFParams m_params;

    QString m_dir;
    std::vector<char> m_labels;
    int m_numOfLetters = 0;

signals:
    void classifiedImageAs(int image_no, char label);
    void treeConstructed();

};

#endif
