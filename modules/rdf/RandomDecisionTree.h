#ifndef RANDOMDESICIONTREE_H
#define RANDOMDESICIONTREE_H

#include <dirent.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <chrono>
#include <random>

#include <3rdparty/cereal/archives/binary.hpp>
#include <3rdparty/cereal/types/memory.hpp>
#include <3rdparty/cereal/types/vector.hpp>
#include <3rdparty/cereal/types/string.hpp>

#include "Util.h"
#include "rdf/PixelCloud.h"
#include "RDFParams.h"
#include "3rdparty/matcerealisation.hpp"
#include "util/Reader.h"

#define MIN_ENTROPY 0.05

#include "Node.h"
#include "DataSet.h"

class RandomDecisionForest;


using treeNodes = tbb::concurrent_vector<node_ptr>;
using rdfclock  = std::chrono::high_resolution_clock;



class RandomDecisionTree : public QObject
{
    Q_OBJECT

  public:
    RandomDecisionTree(RandomDecisionForest *DF);

    RandomDecisionForest *m_DF;
    treeNodes m_nodes;
    quint32 m_minLeafPixelCount;
    int m_height{};
    int m_numOfLeaves{};
    int m_maxDepth{};
    int m_probe_distanceX{};
    int m_probe_distanceY{};


    void train();
    void constructTreeAtDepth(int height);

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

    void computeDivisionAt(Node &node);

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
        // TODO: modify
        node_ptr root = m_nodes[nodeId];
//        assert(root);
//        if(root->m_isLeaf)
//        {
//            // qDebug()<<"LEAF REACHED :"<<root.id;
//            return root;
//        }
//        cv::Mat img = DS.m_testImagesVector[px->sampleId];
//        int childId = root->m_id * 2 ;
//        //qDebug()<<"LEAF SEARCH :"<<root.id << " is leaf : " << root.isLeaf;
//        if(!isLeft(px, *root, img))
//            ++childId;
        return getLeafNode(DS, px, 1);
    }

    bool isPixelSizeConsistent();
    void toString();
    void printTree();

    ~RandomDecisionTree()
    {
        m_nodes.clear();
    }

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive( m_height, m_numOfLeaves, m_maxDepth, m_probe_distanceX,
                 m_probe_distanceY, m_minLeafPixelCount, m_nodes);
    }

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


    inline void divide(const DataSet &DS, QVector<bool> srtVals, Node &node)
    {
        auto rng = node.m_dataRange;
        quint32 leftCount = 0;

        for (int i = rng.m_dx; i < rng.m_dy; ++i) {
            auto px = m_pixelCloud.pixels[i];
            auto img = DS.m_trainImagesVector[px->sampleId];
            auto left = isLeft(px, node, img);
            srtVals[i-rng.m_dx] = left;
            if(left)
                leftCount++;
        }

        m_pixelCloud.putInOrder(rng.m_dx,srtVals);
        node.m_leftCount = leftCount;

    }

    void initNodes();
    void constructTree();

    inline void processNode(node_ptr node, Coord parentData, quint32 leftCount)
    {
        quint32 rightCount = parentData.m_dy - parentData.m_dx - leftCount;
        int mult = (node->m_id+1)%2; // 0 if left, 1 if right
        node->m_dataRange.m_dx = parentData.m_dx + mult*leftCount;
        node->m_dataRange.m_dy = parentData.m_dy - ((mult+1)%2)*rightCount;
        node->m_tau = generateTau();
        generateTeta(node->m_teta1, m_probe_distanceX, m_probe_distanceY);
        generateTeta(node->m_teta2, m_probe_distanceX, m_probe_distanceY);

        computeDivisionAt(*node);
    }

    inline int letterIndex(char letter)
    {
        return letter - 'a';
    }

    inline cv::Mat_<float> computeHistogram(Coord crd, int labelCount)
    {
        cv::Mat_<float> hist(1, labelCount);
        hist.setTo(0.0f);
        for (int pxIndex = crd.m_dx; pxIndex < crd.m_dy; ++pxIndex) {
            pixel_ptr px = m_pixelCloud.pixels[pxIndex];
            int index = letterIndex(px->sampleLabel.at(0).toLatin1());
            ++hist.at<float>(0, index);
        }

        return hist;
    }

    void computeLeafHistograms();
};

using rdt_ptr = std::shared_ptr<RandomDecisionTree>;

#endif // RANDOMDESICIONTREE_H
