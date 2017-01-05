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

class RandomDecisionTree
{
  private:
    quint32 m_minLeafPixelCount;
    quint32 m_height{};
    quint32 m_probe_distanceX{};
    quint32 m_probe_distanceY{};

    std::vector<Node> m_nodes;
    PixelCloud m_pixelCloud;
    DataSet *m_DS;
    QVector<cv::Mat_<float>> m_imgMHOGF{};
    RDFParams *m_params;

    // Random number generators
    std::mt19937 m_generator;
    std::uniform_int_distribution<> m_yProbDistribution;
    std::uniform_int_distribution<> m_xProbDistribution;
//    std::uniform_int_distribution<> m_tauProbDistribution;
    std::uniform_real_distribution<> m_tauProbDistribution;

  public:
    RandomDecisionTree();
    RandomDecisionTree(DataSet *DS, RDFParams *params);
    void inline setGenerator(std::mt19937 &generator) {m_generator = generator;}
    void inline setDataSet(DataSet *DS) {m_DS = DS;}
    void inline setImgMHOGF(QVector<cv::Mat_<float>> imgMHOGF) {m_imgMHOGF = imgMHOGF;}
    void inline setParams(RDFParams *params) {m_params = params;}
    std::mt19937 inline getGenerator() {return m_generator;}
    void train();
    bool isPixelSizeConsistent();

    cv::Mat_<float> inline getProbHist(Pixel &px, cv::Mat_<float>& feature)
    {
        // FIXME: fix this method, it is not working properly
        Node curr = m_nodes[0];
        for (quint32 depth = 1; depth < m_height; ++depth)
            if(isLeft(px, curr, feature))
                curr = m_nodes[2*curr.id + 1];
            else
                curr = m_nodes[2*curr.id + 2];

//        std::cout<< "curr: " << curr.hist << std::endl;
//        std::cout<< "nodes: " << m_nodes[curr.id].hist << std::endl;
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
        m_yProbDistribution   = std::uniform_int_distribution<>(-m_probe_distanceY,
                                                                m_probe_distanceY);
    }

    void inline setMaxDepth(int maxDepth)
    {
        m_height = maxDepth;
    }

    void inline setMinimumLeafPixelCount(quint32 min_leaf_pixel_count)
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

    void inline initParams()
    {
        setProbeDistanceX(m_params->probDistX);
        setProbeDistanceY(m_params->probDistY);
        setMaxDepth(m_params->maxDepth);
        setMinimumLeafPixelCount(m_params->minLeafPixels);
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

        if(isLeaf(m_nodes[index].start, m_nodes[index].end))
        {
            generateTeta(m_nodes[index].teta1);
            generateTeta(m_nodes[index].teta2);
            m_nodes[index].tau = 500; // While rearenging makes pixels move to left
        }
        else
            computeDivisionAt(index);
    }

//    bool inline isLeft(Pixel &p, Node &node, cv::Mat &img)
//    {
//        qint16 new_teta1R = node.teta1.y + p.position.y;
//        qint16 new_teta1C = node.teta1.x + p.position.x;
//        qint16 intensity1 = img.at<uchar>(new_teta1R, new_teta1C);
//        qint16 new_teta2R = node.teta2.y + p.position.y;
//        qint16 new_teta2C = node.teta2.x + p.position.x;
//        qint16 intensity2 = img.at<uchar>(new_teta2R, new_teta2C);
//        return (intensity1 - intensity2) <= node.tau;
//    }

    bool inline isLeft(Pixel &p, Node &node, cv::Mat_<float>& feature)
    {
        // 8 comes from cell size which is (8x8), 9 is binSize
        // FIXME: for point we use (x,y) as (row, col); however for cells teta (col,row)
        qint16 cellx = (node.teta1.x + p.position.x) / 8;
        qint16 celly = (node.teta1.y + p.position.y) / 8;

        qint16 cellx_ = (node.teta2.x + p.position.x) / 8;
        qint16 celly_ = (node.teta2.y + p.position.y) / 8;

        // since each hists length is 9 bin
        cellx = cellx*9;
        cellx_ = cellx_*9;

        if(cellx < 0 || (cellx+8) > feature.cols)
            return false;
        else if(cellx_ < 0 || (cellx_+8) > feature.cols)
            return false;

        cv::Mat_<float> hist1 = feature(cv::Range(celly,celly+1),cv::Range(cellx,cellx+9));
        cv::Mat_<float> hist2 = feature(cv::Range(celly_,celly_+1),cv::Range(cellx_,cellx_+9));
        double dist = cv::compareHist(hist1,hist2,CV_COMP_BHATTACHARYYA);
        return dist <= node.tau;
    }

    bool inline isLeaf(quint32 start, quint32 end)
    {
        if(start == end)
            return true;
        int label = m_pixelCloud.pixels1[start].label;
        int sum = 0;
        for(auto i = start; i < end; ++i)
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

//    int inline generateTau()
//    {
//        // random number between -127, +128
//        return m_tauProbDistribution(m_generator);
//    }

    double inline generateTau()
    {
        // random number between (0,1)
        return m_tauProbDistribution(m_generator);
    }

    cv::Mat_<float> inline computeHistogram(quint32 start, quint32 end, int labelCount)
    {
        cv::Mat_<float> hist(1, labelCount);
        hist.setTo(0.0f);
        for (quint32 pxIndex = start; pxIndex < end; ++pxIndex)
            ++hist(m_pixelCloud.pixels1[pxIndex].label);

        // TODO: might be better if we keep it normalized
//        quint32 tot = cv::sum(hist)[0];
        hist /= (end-start);
//        std::cout<< "curr: " << hist << std::endl;

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

//using rdt_ptr = std::shared_ptr<RandomDecisionTree>;

#endif // RANDOMDESICIONTREE_H
