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

#define MAX_NUMITERATION_FOR_DIVISION 5
#define PIXELS_PER_IMAGE 20
#define MIN_ENTROPY 0.05
#define NUM_LABELS 23
#define MAX_DEPTH 16
#define MIN_LEAF_PIXELS 10




struct Coord{
    int x,y;
    Coord(){x=0;y=0;}
    Coord(int xpos, int ypos) :
        x(xpos), y(ypos) {}
    Coord(const Coord& crd) : x(crd.x), y(crd.x) { } // user-defined copy ctor
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
    int taw;
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
    RandomDecisionForest(int probe_dstanceX,int probe_dstanceY)
    {
        probe_distanceX = probe_dstanceX;
        probe_distanceY = probe_dstanceY;
        srand (time(NULL));
        min_InfoGain = 1;
        max_InfoGain = -1;


    }
    void readTrainingImageFiles();
    void printPixelCloud();
    void printPixel(Pixel* px);
    void generateTeta(Coord& crd);
    int generateTaw();
    float calculateEntropy(cv::Mat& hist);
    float calculateEntropyOfVector(std::vector<Pixel*>& pixels);
    cv::Mat createHistogram(std::vector<Pixel*>& pixels);
    bool isLeft(Pixel* p, Node& node, cv::Mat& img);

    inline void divide(std::vector<Pixel*>& parentPixels, std::vector<Pixel*>& left, std::vector<Pixel*>& right, Node& parent)
    {
        for (auto px : parentPixels)
        {
            auto img = imagesVector[px->imgInfo->sampleId];
            (isLeft(px, parent, img) ? left : right).push_back(px);
        }
    }

    void trainTree();
    void constructTree(Node& root, std::vector<Pixel*>& pixels);
    void tuneParameters(std::vector<Pixel*>& parentPixels, Node& parent);
    int pixelCloudSize();
    void printHistogram(cv::Mat& hist);
    void printNode(Node& node);
    void printTree(RandomDecisionTree tree);
    cv::Mat getPixelImage(Pixel* px);
    Node getLeafNode(Pixel*px, int nodeId);

    inline int getMaxLikelihoodIndex(cv::Mat& hist)
    {
        int max_val=-1;
        int max_index=0;
        for(int i=0;i<hist.cols;i++)
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
    void imageToPixels(std::vector<Pixel*>& res, QString& filePath,ImageInfo* img_inf);
    bool test(std::vector<Pixel*>& letterPixels, char letter);
    // Keep all images on memory
    std::vector<cv::Mat> imagesContainer;
    // Test and Train dirs :
    QString testpath, m_trainpath;

    void setTrainPath(QString path);
    void setTestPath(QString path);
    void trainForest();
    std::vector<cv::Mat> imagesVector;
    std::vector<RandomDecisionTree> m_forest;
    int m_no_of_trees;
    Tree m_tempTree;
    void setNumberofTrees(int no_of_trees){
        m_no_of_trees = no_of_trees;
    }
private:
    QString m_dir;

    std::vector<Pixel*> pixelCloud;
    std::vector<char> m_labels;
    int probe_distanceX, probe_distanceY;
    int m_numOfLetters = 0;
    int numOfLeaves = 0;
    int m_depth;
    float min_InfoGain;
    float max_InfoGain;



    void subSample();
};

#endif
