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
#include <3rdparty/cereal/access.hpp>

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
    quint32 m_height{};
    quint32 m_probe_distanceX{};
    quint32 m_probe_distanceY{};

    std::vector<Node> m_nodes;
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

    inline cv::Mat_<float> getProbHist(Pixel &px, cv::Mat &roi)
    {
        Node &curr = m_nodes[0];
        for (quint32 depth = 1; depth < m_height; ++depth)
            if(isLeft(px, curr, roi))
                curr = m_nodes[2*curr.id + 1];
            else
                curr = m_nodes[2*curr.id + 2];
        return curr.hist;
    }

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
        m_height = maxDepth;
    }

    inline void setMinimumLeafPixelCount(quint32 min_leaf_pixel_count)
    {
        m_minLeafPixelCount = min_leaf_pixel_count;
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

    inline void initParams()
    {
        setProbeDistanceX(m_params->probDistX);
        setProbeDistanceY(m_params->probDistY);
        setMaxDepth(m_params->maxDepth);
        setMinimumLeafPixelCount(m_params->minLeafPixels);
    }

    inline void initNodes()
    {
        auto size = (1ul << m_height) - 1 ;
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

        if(isLeaf(m_nodes[index].start, m_nodes[index].end))
            m_nodes[index].tau = 500; // While rearenging makes pixels move to left
        else
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

    inline bool isLeaf(quint32 start, quint32 end)
    {
        int label = m_pixelCloud.pixels1[start].label;
        int sum = 0;
        for(auto i = start; i < end; ++i)
            sum += std::abs(m_pixelCloud.pixels1[i].label - label);

        bool isPureNode = (sum == 0);
        bool isMinPixReached = (end - start) <= m_minLeafPixelCount;
        return isMinPixReached || isPureNode;
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

    inline cv::Mat_<float> computeHistogram(quint16 start, quint16 end, int labelCount)
    {
        cv::Mat_<float> hist(1, labelCount);
        hist.setTo(0.0f);
        for (int pxIndex = start; pxIndex < end; ++pxIndex)
            ++hist.at<float>(0, m_pixelCloud.pixels1[pxIndex].label);

        // TODO: might be better if we keep it normalized
        int tot = cv::sum(hist)[0];
        if(tot != 0)
            hist /= tot;
        return hist;
    }


private:
    friend class cereal::access;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(m_height, m_probe_distanceX,
                m_probe_distanceY, m_minLeafPixelCount, m_nodes);
    }
};

using rdt_ptr = std::shared_ptr<RandomDecisionTree>;

#endif // RANDOMDESICIONTREE_H
