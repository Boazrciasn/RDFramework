#ifndef RANDOMDESICIONTREE_H
#define RANDOMDESICIONTREE_H

#include <dirent.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <chrono>
#include <random>

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include "Util.h"
#include "PixelCloud.h"
#include "RDFParams.h"
#include "matcerealisation.hpp"
#include "Reader.h"

#define MIN_ENTROPY 0.05

struct Node
{
    qint16 m_tau;
    Coord m_teta1, m_teta2;
    quint32 m_id;
    bool m_isLeaf;
    cv::Mat_<float> m_hist;

    Node() : Node(0, false)
    {
    }

    Node(quint32 id, bool isLeaf): m_id(id), m_isLeaf(isLeaf)
    {
        m_hist.create(1, 1);
        m_hist.setTo(0);
    }

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive( m_tau, m_teta1, m_teta2, m_id, m_isLeaf, m_hist);
    }

    ~Node()
    {
    }
};

struct DataSet
{

    QHash<int, cv::Mat> m_TrainHashTable;
    std::vector<cv::Mat> m_trainImagesVector;
    std::vector<cv::Mat> m_testImagesVector;
    std::vector<QString> m_testlabels;
    std::vector<QString> m_trainlabels;

    ~DataSet()
    {
    }
};

class RandomDecisionForest;

using node_ptr  = std::shared_ptr<Node>;
using TreeNodes = std::vector<node_ptr>;
using rdfclock  = std::chrono::high_resolution_clock;
using rdf_ptr   = std::shared_ptr<RandomDecisionForest>;


class RandomDecisionTree : public QObject
{
    Q_OBJECT

  public:

    ~RandomDecisionTree()
    {
    }

    RandomDecisionTree(rdf_ptr DF);
    RandomDecisionTree(RandomDecisionForest *DF);

    rdf_ptr m_sDF;
    RandomDecisionForest *m_DF;
    int m_depth;
    int m_numOfLeaves;
    int m_maxDepth;
    int m_probe_distanceX, m_probe_distanceY;
    quint32 m_minLeafPixelCount;
    TreeNodes m_nodes;

    void train();
    void constructTree(Node &root, PixelCloud &pixels);

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive( m_depth, m_numOfLeaves, m_maxDepth, m_probe_distanceX,
                 m_probe_distanceY, m_minLeafPixelCount, m_nodes);
    }

    inline void generateTeta(Coord &crd, int probe_x, int probe_y)
    {
        // random number between -probe_distance, probe_distance
        crd.m_dy = m_disProbY(generator);
        crd.m_dx = m_disProbX(generator) ;
    }

    inline int generateTau()
    {
        // random number between -127, +128
        return m_disProbTau(generator);
    }

    inline void setMaxDepth(int max_depth)
    {
        m_maxDepth = max_depth;
        auto size = (1 << m_maxDepth) - 1 ;
        m_nodes.resize(size);
    }

    inline void setProbeDistanceX(int probe_distanceX )
    {
        m_probe_distanceX = probe_distanceX;
        m_disProbX   = std::uniform_int_distribution<>(-m_probe_distanceX,
                                                       m_probe_distanceX);
    }
    inline void setProbeDistanceY(int probe_distanceY )
    {
        m_probe_distanceY = probe_distanceY;
        m_disProbY   = std::uniform_int_distribution<>(-m_probe_distanceY,
                                                       m_probe_distanceY);
    }

    inline void setMinimumLeafPixelCount(unsigned int min_leaf_pixel_count)
    {
        m_minLeafPixelCount = min_leaf_pixel_count;
    }

    void tuneParameters(std::vector<pixel_ptr> &parentPixels, Node &parent);

    inline bool isLeft(pixel_ptr p, Node &node, cv::Mat &img)
    {
        qint16 new_teta1R = node.m_teta1.m_dy + p->position.m_dy;
        qint16 new_teta1C = node.m_teta1.m_dx + p->position.m_dx;
        qint16 intensity1 = img.at<uchar>(new_teta1R, new_teta1C);
        qint16 new_teta2R = node.m_teta2.m_dy + p->position.m_dy ;
        qint16 new_teta2C = node.m_teta2.m_dx + p->position.m_dx ;
        qint16 intensity2 = img.at<uchar>(new_teta2R, new_teta2C);
        return intensity1 - intensity2 <= node.m_tau;
    }

    inline node_ptr getLeafNode(const DataSet &DS, pixel_ptr px, int nodeId)
    {
        node_ptr root = m_nodes[nodeId];
        assert(root);
        if(root->m_isLeaf)
        {
            // qDebug()<<"LEAF REACHED :"<<root.id;
            return root;
        }
        cv::Mat img = DS.m_testImagesVector[px->imgInfo->m_sampleId];
        int childId = root->m_id * 2 ;
        //qDebug()<<"LEAF SEARCH :"<<root.id << " is leaf : " << root.isLeaf;
        if(!isLeft(px, *root, img))
            ++childId;
        return getLeafNode(DS, px, childId - 1);
    }

    bool isPixelSizeConsistent();
    void toString();
    void printTree();

  private:
    PixelCloud m_pixelCloud;
    std::mt19937 generator;
    std::uniform_int_distribution<> m_disProbY;
    std::uniform_int_distribution<> m_disProbX;
    std::uniform_int_distribution<> m_disProbTau;

    void subSample();
    void printPixelCloud();
    void printPixel(pixel_ptr px);
    void printNode(Node &node);


    inline void divide(const DataSet &DS, const PixelCloud &parentPixels,
                       std::vector<pixel_ptr> &left, std::vector<pixel_ptr> &right, Node &parent)
    {
        for (auto px : parentPixels)
        {
            //            auto img = DS.m_trainImagesVector[px->imgInfo->m_sampleId];
            auto img = DS.m_TrainHashTable.value(px->imgInfo->m_sampleId);
            (isLeft(px, parent, img) ? left : right).push_back(px);
        }
    }
};

using rdt_ptr = std::shared_ptr<RandomDecisionTree>;

#endif // RANDOMDESICIONTREE_H
