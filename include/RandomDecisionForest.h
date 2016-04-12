#ifndef CPV_RANDOM_DECISION_FOREST
#define CPV_RANDOM_DECISION_FOREST


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "QString"
#include <QDir>
#include <QDebug>
#include "QApplication"
#include <QDirIterator>
#include <include/Util.h>

#include <dirent.h>
#include <iostream>

#include <time.h>       /* time */

#define MAX_NUMITERATION_FOR_DIVISION 10
#define PIXELS_PER_IMAGE 50
#define MIN_ENTROPY 0.05
#define NUM_LABELS 26 // should make it generic according to training input
#define MAX_DEPTH 16
#define MIN_LEAF_PIXELS 20




struct Coord{
    int m_x,m_y;
    Coord(){m_x=0;m_y=0;}
    Coord(int x, int y) :
        m_x(x), m_y(y) {}
    Coord(const Coord& crd) : m_x(crd.m_x), m_y(crd.m_x) {}
};

struct ImageInfo
{
    char label;
    int sampleId;
    ImageInfo(char lbl, int smplId) :
        label(lbl), sampleId(smplId){}
};

struct Pixel
{
    Coord position;
    unsigned char intensity;
    ImageInfo* imgInfo;
    Pixel(Coord crd, unsigned char intnsty, ImageInfo* imgInf):
        position(crd), intensity(intnsty),imgInfo(imgInf){}
};

struct Node
{
    int tau;
    Coord teta1, teta2;
    bool isLeaf;
    int id;
    cv::Mat hist;
};

using Tree = std::vector<Node>;

class RandomDecisionTree
{

public:
    RandomDecisionTree() :  m_tree(pow(2, MAX_DEPTH)-1){}
    int m_depth;
    int m_numberofleaves;
    Tree m_tree;
};

class RandomDecisionForest{

public:

    RandomDecisionForest(int probe_dstanceX,int probe_dstanceY) : m_tempTree(pow(2, MAX_DEPTH)-1)
    {
        m_probe_distanceX = probe_dstanceX;
        m_probe_distanceY = probe_dstanceY;
        srand (time(NULL));
        min_InfoGain = 1;
        max_InfoGain = -1;

    }

    void readTrainingImageFiles();
    void readTestImageFiles();
    void printPixelCloud();
    void printPixel(Pixel* px);
    void generateTeta(Coord& crd);
    int generateTau();
    float calculateEntropy(cv::Mat& hist);
    float calculateEntropyOfVector(std::vector<Pixel*>& pixels);
    cv::Mat createHistogram(std::vector<Pixel*>& pixels);
    bool isLeft(Pixel* p, Node& node, cv::Mat& img);

    inline void divide(std::vector<Pixel*>& parentPixels, std::vector<Pixel*>& left, std::vector<Pixel*>& right, Node& parent)
    {
        for (auto px : parentPixels)
        {
            auto img = m_trainImagesVector[px->imgInfo->sampleId];
            (isLeft(px, parent, img) ? left : right).push_back(px);
        }
    }

    void trainTree();
    bool pixelSizesConsistent();

    inline int getTotalNumberOfPixels(const cv::Mat& hist)
    {
        int totalSize =0;
        int nCols = hist.cols;
        for(int i=0; i<nCols; ++i)
            totalSize += hist.at<float>(0, i);
        return totalSize;
    }

    cv::Mat classify(int index);
    void test();
    bool test(const cv::Mat& image, char letter, const Tree &tree);

    void constructTree(Node& root, std::vector<Pixel*>& pixels);
    void tuneParameters(std::vector<Pixel*>& parentPixels, Node& parent);
    int pixelCloudSize();
    void printHistogram(cv::Mat& hist);
    void printNode(Node& node);
    void printTree(RandomDecisionTree tree);
    cv::Mat getPixelImage(Pixel* px);
    Node getLeafNode(Pixel*px, int nodeId, const Tree &tree);

    inline int getMaxLikelihoodIndex(const cv::Mat& hist)
    {
        int max_val=-1;
        int max_index=0;
        for(int i=0;i<hist.cols;++i)
        {
            if(hist.at<float>(0, i) > max_val)
            {
                max_val = hist.at<float>(0, i);
                max_index = i;
            }
        }
        return max_index;
    }

    inline int letterIndex(char letter)
    {
        return letter-'a';
    }

    void imageToPixels(std::vector<Pixel*>& res, const cv::Mat &image,ImageInfo* img_inf);
    cv::Mat colorCoder(const cv::Mat &labelImage, const cv::Mat &InputImage);
    void setTrainPath(QString path);
    void setTestPath(QString path);
    void trainForest();

    void setNumberofTrees(int no_of_trees){
        m_no_of_trees = no_of_trees;
    }

    std::vector<Pixel*> pixelCloud;
    std::vector<cv::Mat> m_trainImagesVector;
    std::vector<cv::Mat> m_testImagesVector;
    std::vector<RandomDecisionTree> m_forest;
    int m_no_of_trees;
    Tree m_tempTree;
    std::vector<Pixel*> m_pixelCloud;
    // Keep all images on memory
    std::vector<cv::Mat> m_imagesContainer;
    // Test and Train dirs :
    QString m_testpath, m_trainpath;
private:

    void subSample();

    QString m_dir;
    std::vector<char> m_labels;
    int m_probe_distanceX, m_probe_distanceY;
    int m_numOfLetters = 0;
    int m_numOfLeaves ;
    int m_depth;
    float min_InfoGain;
    float max_InfoGain;
};

#endif
