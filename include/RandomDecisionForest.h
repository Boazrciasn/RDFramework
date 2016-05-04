#ifndef CPV_RANDOM_DECISION_FOREST
#define CPV_RANDOM_DECISION_FOREST

#include "include/RandomDecisionTree.h"

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
        for(auto *DT : m_forest)
            delete DT;
        m_forest.clear();
    }

    void readTrainingImageFiles();
    void readTestImageFiles();
    void printPixelCloud();
    void printPixel(pixel_ptr px);
    int pixelCloudSize();
    cv::Mat getPixelImage(pixel_ptr px);
    void imageToPixels(std::vector<pixel_ptr>& res, const cv::Mat &image,imageinfo_ptr img_inf);
    cv::Mat colorCoder(const cv::Mat &labelImage, const cv::Mat &InputImage);
    void trainForest();
    void test();
    cv::Mat getLayeredHist(cv::Mat test_image, int index);
    RDFParams &params() { return m_params; }
    void setParams(const RDFParams &params) { m_params = params; }
    DataSet m_DS;
    std::vector<RandomDecisionTree *> m_forest;
    // Keep all images on memory
    std::vector<cv::Mat> m_imagesContainer;
    inline void setParentWidget(QWidget *parent_widget)
    {
        m_parent = parent_widget;
    }
    QWidget* m_parent;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive( m_params, m_dir, m_numOfLetters);
    }


private:
//    rdfclock::time_point m_begin;

    void placeHistogram(cv::Mat &output, const cv::Mat &pixelHist, int pos_row, int pos_col);
    cv::Mat createLetterConfidenceMatrix(const cv::Mat &layeredHist);
    double m_accuracy;
    std::vector<QString> classify_res;
    RDFParams m_params;
    QString m_dir;
    int m_numOfLetters = 0;

signals:
    void classifiedImageAs(int image_no, char label);
    void treeConstructed();
    void resultPercentage(double accuracy);

};

#endif
