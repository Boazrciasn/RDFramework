#ifndef CPV_RANDOM_DECISION_FOREST
#define CPV_RANDOM_DECISION_FOREST

#include <dirent.h>
#include <iostream>
#include <time.h>

#include "include/Util.h"
#include "include/PixelCloud.h"
#include "include/RDFParams.h"

#define MIN_ENTROPY 0.05

struct Node
{
    qint16 m_tau;
    Coord m_teta1, m_teta2;
    quint32 m_id;
    bool m_isLeaf;
    cv::Mat m_hist;
    Node(quint32 id, bool isLeaf):
        m_id(id), m_isLeaf(isLeaf)
    {
    }
};

using TreeNodes = std::vector<Node*>;

struct DataSet
{
    std::vector<cv::Mat> m_trainImagesVector;
    std::vector<cv::Mat> m_testImagesVector;
};

class RandomDecisionForest;

class RandomDecisionTree : public QObject
{
    Q_OBJECT

public:
    void train(PixelCloud &pixelCloud);

    RandomDecisionTree(RandomDecisionForest *DF) : m_DF(DF) { }

    RandomDecisionForest *m_DF;
    int m_depth;
    int m_numOfLeaves;
    int m_maxDepth;
    int m_probe_distanceX, m_probe_distanceY;

    TreeNodes m_nodes;

    void constructTree(Node& root, std::vector<Pixel*>& pixels);

    inline void setMaxDepth(int max_depth)
    {
        m_maxDepth = max_depth;
        m_nodes.resize((1 << m_maxDepth)-1);
    }

    inline void setProbeDistanceX(int probe_distanceX ) { m_probe_distanceX = probe_distanceX; }
    inline void setProbeDistanceY(int probe_distanceY ) { m_probe_distanceY = probe_distanceY; }

    inline void setMinimumLeafPixelCount(unsigned int min_leaf_pixel_count) { m_minLeafPixelCount = min_leaf_pixel_count; }

    void tuneParameters(std::vector<Pixel*>& parentPixels, Node& parent);

    inline bool isLeft(Pixel* p, Node& node, cv::Mat& img)
    {
        qint16 new_teta1R = node.m_teta1.m_dy + p->position.m_dy;
        qint16 new_teta1C = node.m_teta1.m_dx + p->position.m_dx;
        qint16 intensity1 = img.at<uchar>(new_teta1R,new_teta1C);

        qint16 new_teta2R = node.m_teta2.m_dy + p->position.m_dy ;
        qint16 new_teta2C = node.m_teta2.m_dx + p->position.m_dx ;
        qint16 intensity2 = img.at<uchar>(new_teta2R,new_teta2C);

        return intensity1 - intensity2 <= node.m_tau;
    }

    inline Node* getLeafNode(const DataSet &DS, Pixel*px, int nodeId)
    {
        Node* root = m_nodes[nodeId];
        assert( root != NULL);
        if(root->m_isLeaf)
        {
            // qDebug()<<"LEAF REACHED :"<<root.id;
            return root;
        }
        cv::Mat img = DS.m_testImagesVector[px->imgInfo->sampleId];
        int childId = root->m_id *2 ;
        //qDebug()<<"LEAF SEARCH :"<<root.id << " is leaf : " << root.isLeaf;
        if(!isLeft(px,*root,img))
            ++childId;
        return getLeafNode(DS, px,childId-1);
    }

    bool isPixelSizeConsistent();

    void toString();

private:
    quint32 m_minLeafPixelCount;

    void subSample();

    inline void divide(const DataSet& DS, const PixelCloud& parentPixels, std::vector<Pixel*>& left, std::vector<Pixel*>& right, Node& parent)
    {
        for (auto px : parentPixels)
        {
            auto img = DS.m_trainImagesVector[px->imgInfo->sampleId];
            (isLeft(px, parent, img) ? left : right).push_back(px);
        }
    }

signals:
    void treeConstructed();
};

class RandomDecisionForest : public QObject
{
    Q_OBJECT

    friend class RandomDecisionTree;

public:
    RandomDecisionForest()
    {
        srand(time(NULL));
        min_InfoGain = 1;
        max_InfoGain = -1;
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
    void printNode(Node& node);
    cv::Mat getPixelImage(Pixel* px);

    void imageToPixels(std::vector<Pixel*>& res, const cv::Mat &image,ImageInfo* img_inf);
    cv::Mat colorCoder(const cv::Mat &labelImage, const cv::Mat &InputImage);
    void setTrainPath(QString path);
    void setTestPath(QString path);
    void trainForest();

    void test();
    cv::Mat classify(int index);

    void setNumberofTrees(int no_of_trees) { m_no_of_trees = no_of_trees; }
    void setNumberofLabels(int no_of_labels) { m_labelCount = no_of_labels; }
    void setMaxIterationForDivision(int max_iteration) { m_maxIterationForDivision = max_iteration; }
    void setPixelsPerImage(int pixel_per_image_count) { m_pixelsPerImage = pixel_per_image_count; }

    RDFParams &params() { return m_params; }
    void setParams(const RDFParams &params) { m_params = params; }

    DataSet m_DS;

    std::vector<RandomDecisionTree *> m_forest;
    int m_no_of_trees;
    PixelCloud m_pixelCloud;
    // Keep all images on memory
    std::vector<cv::Mat> m_imagesContainer;

    QString m_testpath;
    QString m_trainpath;

private:
    RDFParams m_params;

    QString m_dir;
    std::vector<char> m_labels;
    int m_numOfLetters = 0;

    float min_InfoGain;
    float max_InfoGain;
    int m_pixelsPerImage;
    int m_labelCount;
    int m_maxIterationForDivision;
    int m_probe_distanceX, m_probe_distanceY;

signals:
    void classifiedImageAs(int image_no, char label);

};

#endif
