#ifndef RANDOMDESICIONTREE_H
#define RANDOMDESICIONTREE_H

#include <dirent.h>
#include <iostream>
#include <time.h>

#include <cereal/archives/binary.hpp>
#include <include/matcerealisation.hpp>
#include <fstream>
#include <chrono>
#include <random>

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

    Node() : Node(0,false)
    {
    }

    Node(quint32 id, bool isLeaf): m_id(id), m_isLeaf(isLeaf)
    {
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive( m_tau, m_teta1, m_teta2, m_id, m_isLeaf, m_hist);
    }
};


using node_ptr = std::shared_ptr<Node>;
using TreeNodes = std::vector<node_ptr>;
using rdfclock =  std::chrono::high_resolution_clock;

struct DataSet
{
    std::vector<cv::Mat> m_trainImagesVector;
    std::vector<cv::Mat> m_testImagesVector;
    std::vector<QString> m_testlabels;
    std::vector<QString> m_trainlabels;
};

class RandomDecisionForest;

class RandomDecisionTree : public QObject
{
    Q_OBJECT

public:
    RandomDecisionTree(RandomDecisionForest *DF) : m_DF(DF)
    {
//        rdfclock::time_point beginning = rdfclock::now();
//        rdfclock::duration d = rdfclock::now()-beginning;
        std::random_device rd;
        generator = std::mt19937(rd());
        m_disProbTau = std::uniform_int_distribution<>(-127, 128);

        //generator = new std::mt19937(d.count());
    }

    RandomDecisionForest *m_DF;
    int m_depth;
    int m_numOfLeaves;
    int m_maxDepth;
    int m_probe_distanceX, m_probe_distanceY;
    TreeNodes m_nodes;

    void train();
    void constructTree(Node& root, PixelCloud &pixels);

    inline void generateTeta(Coord& crd, int probe_x, int probe_y)
    {
        // random number between -probe_distance, probe_distance
        //std::uniform_int_distribution<> dis(-probe_y, probe_y); //[]
        crd.m_dy = m_disProbY(generator);

        //std::uniform_int_distribution<> dis2(-probe_x, probe_x); //[]
        crd.m_dx = m_disProbX(generator) ;

//        crd.m_dy = (rand() % (2*probe_y)) - probe_y;
//        crd.m_dx = (rand() % (2*probe_x)) - probe_x;
    }

    inline int generateTau()
    {
        // random number between -127, +128
        //std::uniform_int_distribution<> dis(-127, 128); //[]
        return m_disProbTau(generator);
//        return (rand() % 256) - 127;
    }


    inline void saveNode(const Node& n)
    {
        std::ofstream file("file.bin", std::ios::binary);
        cereal::BinaryOutputArchive ar(file);
        ar(n);
    }

    inline void loadNode(Node& n)
    {
        std::ifstream file("file.bin", std::ios::binary);
        cereal::BinaryInputArchive ar(file);
        ar(n);
    }

    inline void setMaxDepth(int max_depth)
    {
        m_maxDepth = max_depth;
        m_nodes.resize((1 << m_maxDepth)-1);
    }

    inline void setProbeDistanceX(int probe_distanceX )
    {
        m_probe_distanceX = probe_distanceX;
        m_disProbX   = std::uniform_int_distribution<>(-m_probe_distanceX, m_probe_distanceX);
    }
    inline void setProbeDistanceY(int probe_distanceY )
    {
        m_probe_distanceY = probe_distanceY;
        m_disProbY   = std::uniform_int_distribution<>(-m_probe_distanceY, m_probe_distanceY);
    }

    inline void setMinimumLeafPixelCount(unsigned int min_leaf_pixel_count) { m_minLeafPixelCount = min_leaf_pixel_count; }

    void tuneParameters(std::vector<pixel_ptr>& parentPixels, Node& parent);

    inline bool isLeft(pixel_ptr p, Node& node, cv::Mat& img)
    {
        qint16 new_teta1R = node.m_teta1.m_dy + p->position.m_dy;
        qint16 new_teta1C = node.m_teta1.m_dx + p->position.m_dx;
        qint16 intensity1 = img.at<uchar>(new_teta1R,new_teta1C);

        qint16 new_teta2R = node.m_teta2.m_dy + p->position.m_dy ;
        qint16 new_teta2C = node.m_teta2.m_dx + p->position.m_dx ;
        qint16 intensity2 = img.at<uchar>(new_teta2R,new_teta2C);

        return intensity1 - intensity2 <= node.m_tau;
    }

    inline node_ptr getLeafNode(const DataSet &DS, pixel_ptr px, int nodeId)
    {
        node_ptr root = m_nodes[nodeId];
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
    PixelCloud m_pixelCloud;
    std::mt19937 generator;
    std::uniform_int_distribution<> m_disProbY;
    std::uniform_int_distribution<> m_disProbX;
    std::uniform_int_distribution<> m_disProbTau;

    void subSample();

    inline void divide(const DataSet& DS, const PixelCloud& parentPixels, std::vector<pixel_ptr>& left, std::vector<pixel_ptr>& right, Node& parent)
    {
        for (auto px : parentPixels)
        {
            auto img = DS.m_trainImagesVector[px->imgInfo->sampleId];
            (isLeft(px, parent, img) ? left : right).push_back(px);
        }
    }

    void printPixelCloud();
    void printPixel(pixel_ptr px);
    void printNode(Node& node);
};

#endif // RANDOMDESICIONTREE_H
