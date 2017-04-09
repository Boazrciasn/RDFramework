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
#include "3rdparty/pcg-cpp-0.98/include/pcg_random.hpp"

#include "Util.h"
#include "rdf/PixelCloud.h"
#include "3rdparty/matcerealisation.hpp"
#include "util/Reader.h"
#include "util/SignalSenderInterface.h"
#include "StatisticsLogger.h"

#define MIN_ENTROPY 0.05

#include "Node.h"
#include "DataSet.h"
#include "RDFParams.h"

using rdfclock  = std::chrono::high_resolution_clock;

class RandomDecisionTree
{
  private:
    quint32 m_minLeafPixelCount;
    quint32 m_height{};
    quint32 m_probe_distanceX{};
    quint32 m_probe_distanceY{};

    std::vector<Node> m_nodes;
    tbb::concurrent_vector<int> m_featureFreq;
    PixelCloud m_pixelCloud;
    DataSet *m_DS;
    RDFParams *m_params;
    StatisticsLogger m_statLog;

    // stat var's :
    quint32 m_nonLeafpxCount;
    quint32 m_leafCount{};

    void calculateImpurity(quint32 d);

    // Random number generators
    pcg32 m_generator;
    std::uniform_int_distribution<> m_yProbDistribution;
    std::uniform_int_distribution<> m_xProbDistribution;
    std::uniform_int_distribution<> m_tauProbDistribution;


  public:
    SignalSenderInterface *m_signalsender;
    RandomDecisionTree() { }
    RandomDecisionTree(DataSet *DS, RDFParams *params);
    void inline setGenerator(pcg32 &generator) {m_generator = generator;}
    void inline setDataSet(DataSet *DS) {m_DS = DS;}
    void inline setParams(RDFParams *params) {m_params = params;}
    void setSignalInterface(SignalSenderInterface *signalsender) {m_signalsender = signalsender;}
    pcg32 inline getGenerator() {return m_generator;}
    void train();
    bool isPixelSizeConsistent();

    cv::Mat_<float> inline getProbHist(Pixel &px, cv::Mat &roi)
    {
        auto curr = m_nodes[0];
        for (quint32 depth = 1; depth < m_height; ++depth)
            if (curr.isLeft(px, roi))
                curr = m_nodes[2 * curr.id + 1];
            else
                curr = m_nodes[2 * curr.id + 2];
        return curr.hist;
    }

    void inline setProbeDistanceX(int probe_distanceX)
    {
        m_probe_distanceX = probe_distanceX;
        m_xProbDistribution   = std::uniform_int_distribution<>(-m_probe_distanceX,
                                                                m_probe_distanceX);
    }

    void inline setProbeDistanceY(int probe_distanceY)
    {
        m_probe_distanceY = probe_distanceY;
        m_yProbDistribution   = std::uniform_int_distribution<>(-m_probe_distanceY, m_probe_distanceY);
    }

    void inline setMaxDepth(int maxDepth)
    {
        m_height = maxDepth;
    }

    void inline setMinimumLeafPixelCount(quint32 min_leaf_pixel_count)
    {
        m_minLeafPixelCount = min_leaf_pixel_count;
    }

    void inline setTauRange(int tauRange)
    {
        m_tauProbDistribution = std::uniform_int_distribution<>(-tauRange, tauRange);
    }

    void inline setTauRangePos(int tauRange)
    {
        m_tauProbDistribution = std::uniform_int_distribution<>(0, tauRange);
    }

    ~RandomDecisionTree()
    {
        m_nodes.clear();
    }


  private:
    void getSubSampleSingleFrame();
    void getSubSample();
    void constructTree();
    void constructRootNode();
    void constructTreeDecisionNodes();
    void computeLeafHistograms();
    void computeDivisionAt(quint32 index);
    void computeDivisionWithLookUpAt(quint32 index);
    void rearrange(quint32 index);

    void inline initParams()
    {
        setProbeDistanceX(m_params->probDistX);
        setProbeDistanceY(m_params->probDistY);
        setMaxDepth(m_params->maxDepth);
        setMinimumLeafPixelCount(m_params->minLeafPixels);
        if(m_params->isPositiveRange)
            setTauRangePos(m_params->tauRange);
        else
            setTauRange(m_params->tauRange);
    }

    void inline initNodes()
    {
        auto size = (1ul << m_height) - 1 ;
        m_nodes.resize(size);
    }

    void inline processNode(quint32 index)
    {
        int mult = (index + 1) % 2; // 0 if left, 1 if right
        int parentId = (index + 1) / 2 - 1;
        quint32 leftCount = m_nodes[parentId].leftCount;
        quint32 rightCount = m_nodes[parentId].end - m_nodes[parentId].start - leftCount;
        m_nodes[index].id = index;
        m_nodes[index].start = m_nodes[parentId].start + mult * leftCount;
        m_nodes[index].end = m_nodes[parentId].end - ((mult + 1) % 2) * rightCount;

        auto pxCount = m_nodes[index].end - m_nodes[index].start;
        if (pxCount == 0)
        {
            return;
//            if (mult == 0)
//                m_nodes[parentId].tau = -500; // Supress all nodes to right if left node is empty
//            else
//                m_nodes[parentId].tau = 500;
        }
        else if (m_nodes[parentId].tau == 500)
        {
            ++m_leafCount;
            m_nonLeafpxCount = m_nonLeafpxCount - pxCount;
            generateTeta(m_nodes[index].teta1);
            generateTeta(m_nodes[index].teta2);
            m_nodes[index].tau = 500;
        }

        else
        {
            if (isLeaf(m_nodes[index].start, m_nodes[index].end))
            {
                ++m_leafCount;
                m_nonLeafpxCount = m_nonLeafpxCount - pxCount;
                generateTeta(m_nodes[index].teta1);
                generateTeta(m_nodes[index].teta2);
                m_nodes[index].tau = 500;
            }
            else
            {
                if(pxCount >= TableLookUp::size)
                    computeDivisionAt(index);
                else
                    computeDivisionWithLookUpAt(index);
                ++m_featureFreq[m_nodes[index].ftrID];
            }
        }
    }

    bool inline isLeaf(quint32 start, quint32 end)
    {
        int label = m_pixelCloud.pixels1[start].label;
        int sum = 0;
        for (auto i = start; i < end; ++i)
            sum += std::abs(m_pixelCloud.pixels1[i].label - label);
        bool isPureNode = (sum == 0);
        bool isMinPixReached = (end - start) <= m_minLeafPixelCount;
        return isMinPixReached || isPureNode;
    }

    void inline generateTeta(cv::Point &crd)
    {
        crd.y = m_yProbDistribution(m_generator);
        crd.x = m_xProbDistribution(m_generator) ;
    }

    int inline generateTau()
    {
        // random number between -127, +128
        return m_tauProbDistribution(m_generator);
    }

    cv::Mat_<float> inline computeHistogramNorm(quint32 start, quint32 end, int labelCount)
    {
        cv::Mat_<float> hist(1, labelCount);
        hist.setTo(0.0f);
        for (quint32 pxIndex = start; pxIndex < end; ++pxIndex)
            ++hist(m_pixelCloud.pixels1[pxIndex].label);
        hist /= (end - start);
        return hist;
    }

    QVector<quint32> inline computeHistogram(quint32 start, quint32 end, int labelCount)
    {
        QVector<quint32> hist(labelCount);
        hist.fill(0);
        for (auto pxIndex = start; pxIndex < end; ++pxIndex)
            ++hist[m_pixelCloud.pixels1[pxIndex].label];
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

    inline void generateParams(quint32 index)
    {
        generateTeta(m_nodes[index].teta1);
        generateTeta(m_nodes[index].teta2);
        m_nodes[index].tau = generateTau();
    }

    inline void computeHistograms(quint32 index, QVector<quint32>& rightHist, QVector<quint32>& leftHist)
    {
        leftHist.fill(0);
        rightHist.fill(0);
        auto sizeLeft  = 0;
        auto sizeRight = 0;
        auto end = m_nodes[index].end;

        for (auto i = m_nodes[index].start; i < end; ++i)
        {
            auto &px = m_pixelCloud.pixels1[i];
            auto &img = m_DS->images[px.id];
            if (m_nodes[index].isLeft(px, img))
            {
                ++leftHist[px.label];
                sizeLeft++;
            }
            else
            {
                ++rightHist[px.label];
                sizeRight++;
            }
        }
    }

    inline void setParamsFor(auto maxFeatureIndex, cv::Point maxTeta1, cv::Point maxTeta2, quint32 index, auto maxTau)
    {
        m_nodes[index].tau = maxTau;
        m_nodes[index].teta1 = maxTeta1;
        m_nodes[index].teta2 = maxTeta2;
        m_nodes[index].ftrID = maxFeatureIndex;
    }
};

#endif // RANDOMDESICIONTREE_H
