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
#include "3rdparty/matcerealisation.hpp"
#include "util/Reader.h"

#define MIN_ENTROPY 0.05

#include "Node.h"
#include "DataSet.h"
#include "RDFParams.h"

using rdfclock  = std::chrono::high_resolution_clock;

class RandomDecisionTree : public QObject
{
    Q_OBJECT
  private:
    quint32 m_minLeafPixelCount;
    quint32 m_hight{};
    quint32 m_probe_distanceX{};
    quint32 m_probe_distanceY{};

    QVector<Node> m_nodes;
    PixelCloud m_pixelCloud;
    DataSet *m_DS;
    RDFParams *m_params;

    // Random number generators
    std::mt19937 generator;
    std::uniform_int_distribution<> m_yProbDistribution;
    std::uniform_int_distribution<> m_xProbDistribution;
    std::uniform_int_distribution<> m_tauProbDistribution;

  signals:
    void treeConstructed();

  public:
    RandomDecisionTree(DataSet *DS, RDFParams *params);
    void train();
    void printTree();

    inline void setProbeDistanceX(int probe_distanceX)
    {
        m_probe_distanceX = probe_distanceX;
        m_xProbDistribution   = std::uniform_int_distribution<>(-m_probe_distanceX,
                                                                m_probe_distanceX);
    }

    inline void setProbeDistanceY(int probe_distanceY)
    {
        m_probe_distanceY = probe_distanceY;
        m_yProbDistribution   = std::uniform_int_distribution<>(-m_probe_distanceY,
                                                                m_probe_distanceY);
    }

    inline void setMaxDepth(int maxDepth)
    {
        m_hight = maxDepth;
    }

    inline void setMinimumLeafPixelCount(quint32 min_leaf_pixel_count)
    {
        m_minLeafPixelCount = min_leaf_pixel_count;
    }

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(m_hight, m_probe_distanceX,
                m_probe_distanceY, m_minLeafPixelCount, m_nodes);
    }

    ~RandomDecisionTree()
    {
        m_nodes.clear();
    }
  private:
    void getSubSample();

    void constructTree();
    void constructRootNode();
    void constructTreeDecisionNodes();
    void computeLeafHistograms();
    void computeDivisionAt(quint32 index);
    void rearrange(quint32 index);
    bool isPixelSizeConsistent();

    void toString();
    void printPixelCloud();
    void printPixel(Pixel &px);
    void printNode(Node &node);

    inline void initNodes()
    {
        auto size = (1ul << m_hight) - 1 ;
        m_nodes.resize(size);
    }

    inline void processNode(quint32 index)
    {
        int mult = (index + 1) % 2; // 0 if left, 1 if right
        int parentId = (index + 1) / 2 - 1;
        quint32 leftCount = m_nodes[parentId].leftCount;
        quint32 rightCount = m_nodes[parentId].end - m_nodes[parentId].start - leftCount;
        m_nodes[index].id = index;
        m_nodes[index].start = m_nodes[parentId].start + mult * leftCount;
        m_nodes[index].end = m_nodes[parentId].end - ((mult + 1) % 2) * rightCount;
        m_nodes[index].tau = generateTau();
        generateTeta(m_nodes[index].teta1);
        generateTeta(m_nodes[index].teta2);
        computeDivisionAt(index);
    }

    inline bool isLeft(Pixel &p, Node &node, cv::Mat &img)
    {
        qint16 new_teta1R = node.teta1.y + p.position.y;
        qint16 new_teta1C = node.teta1.x + p.position.x;
        qint16 intensity1 = img.at<uchar>(new_teta1R, new_teta1C);
        qint16 new_teta2R = node.teta2.y + p.position.y;
        qint16 new_teta2C = node.teta2.x + p.position.x;
        qint16 intensity2 = img.at<uchar>(new_teta2R, new_teta2C);
        return (intensity1 - intensity2) <= node.tau;
    }

    inline void generateTeta(cv::Point &crd)
    {
        crd.y = m_yProbDistribution(generator);
        crd.x = m_xProbDistribution(generator) ;
    }

    inline int generateTau()
    {
        // random number between -127, +128
        return m_tauProbDistribution(generator);
    }

    inline int letterIndex(char letter)
    {
        return letter - 'a';
    }

    inline cv::Mat_<float> computeHistogram(quint16 start, quint16 end, int labelCount)
    {
        cv::Mat_<float> hist(1, labelCount);
        hist.setTo(0.0f);
        for (int pxIndex = start; pxIndex < end; ++pxIndex)
        {
            Pixel px = m_pixelCloud.pixels1[pxIndex];
            ++hist.at<float>(0, px.label);
        }
        return hist;
    }
};

using rdt_ptr = std::shared_ptr<RandomDecisionTree>;

#endif // RANDOMDESICIONTREE_H
