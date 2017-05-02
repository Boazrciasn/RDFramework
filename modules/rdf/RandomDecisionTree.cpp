#include "RandomDecisionTree.h"


void RandomDecisionTree::calculateImpurity(quint32 d)
{
    quint32 impurity = 0;
    int level_node_count = 1 << d;          // Number of nodes at this level
    int tot_node_count   = (1 << (d + 1)) - 1; // Number of nodes at this and previous levels
    for (auto node_id = (tot_node_count - level_node_count); node_id < tot_node_count; ++node_id)
    {
        float entropy = calculateEntropyProb(computeHistogramNorm(m_nodes[node_id].start, m_nodes[node_id].end,
                                                                  m_params->labelCount));
        impurity += (m_nodes[node_id].end - m_nodes[node_id].start) * entropy;
    }
    m_statLog.logImpurity(impurity, d);
}

void RandomDecisionTree::train()
{
    // Intit zero freq
    m_featureFreq.resize(Feature::features.size());
    std::fill (m_featureFreq.begin(),m_featureFreq.end(),0);

    auto begin = std::chrono::high_resolution_clock::now();
    SignalSenderInterface::instance().printsend("Initializing Parameters...");
    initParams();
    SignalSenderInterface::instance().printsend("Initializing nodes...");
    initNodes();
    m_statLog.setDepth(m_params->maxDepth - 1);
    SignalSenderInterface::instance().printsend("Sub-sampling...");
//    getSubSample();
    getSubSampleSingleFrame(); // TODO: put if statement here
    SignalSenderInterface::instance().printsend("Constructing tree...");
    qApp->processEvents();
    constructTree();

    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::seconds>(end-begin).count();
    m_statLog.logFeatureFreq(m_featureFreq);
    m_statLog.logTrainingTime(time);
    m_statLog.dump();
    qApp->processEvents();

    compreseTree();
}

void RandomDecisionTree::getSubSampleSingleFrame()
{
    quint32 imgCount = m_DS->images.size();
    for (quint32 id = 0; id < imgCount; ++id)
    {
        auto &image = m_DS->images[id];
        auto label  = 0; // TODO: m_DS->labels[id];
        int nRows = image.rows;
        int nCols = image.cols;

        auto& rects = m_DS->frameRects[id];
        auto rectCount = rects.size();

        // Positive label
        for (int i = 0; i < rectCount; ++i)
        {
            for (int k = 0; k < m_params->pixelsPerImage; ++k)
            {
                int row = (m_generator() % rects[i].height()) + rects[i].y() + m_probe_distanceY + Feature::max_h;
                int col = (m_generator() % rects[i].width()) + rects[i].x() + m_probe_distanceX + Feature::max_w;
                Pixel px(cv::Point(col,row),id, label);
                m_pixelCloud.pixels1.push_back(px);
            }
        }

        label = 1;
        // Nagative label
        for (int k = 0; k < m_params->pixelsPerImage*rectCount; ++k)
        {
            int row = (m_generator() % (nRows - 2 * (m_probe_distanceY + Feature::max_h))) + m_probe_distanceY + Feature::max_h;
            int col = (m_generator() % (nCols - 2 * (m_probe_distanceX + Feature::max_w))) + m_probe_distanceX + Feature::max_w;

            bool isNeg = true;
            for (int i = 0; i < rectCount; ++i)
                if(row > rects[i].y() && row < (rects[i].y() + rects[i].height()) && col > rects[i].x() && col < (rects[i].x() + rects[i].width()))
                {
                    isNeg = false;
                    break;
                }

            if(!isNeg)// TODO: || row < 180)
            {
                --k;
                continue;
            }

            Pixel px(cv::Point(col,row),id, label);
            m_pixelCloud.pixels1.push_back(px);
        }
    }

    m_pixelCloud.pixels2.resize(m_pixelCloud.pixels1.size());
}

void RandomDecisionTree::getSubSample()
{
    quint32 imgCount = m_DS->images.size();
    // Set Proper size
    auto size = 0;
    for (auto it = std::begin(m_DS->map_dataPerLabel); it != std::end(m_DS->map_dataPerLabel); ++it)
        size += (it->second * m_params->pixelsPerLabelImage[it->first]);
    m_pixelCloud.pixels1.resize(size);
    m_pixelCloud.pixels2.resize(size);
    quint32 last{};
    for (quint32 id = 0; id < imgCount; ++id)
    {
        auto &image = m_DS->images[id];
        auto label  = m_DS->labels[id];
        int nRows = image.rows;
        int nCols = image.cols;

        auto sum = cv::sum(image)[0];
        quint16 pxCount = m_params->pixelsPerLabelImage[label];
        for (int k = 0; k < pxCount; ++k)
        {
            int row = 0;
            int col = 0;
            quint8 intensity = 0;
            while (intensity == 0)
            {
                row = (m_generator() % (nRows - 2 * (m_probe_distanceY + Feature::max_h))) + m_probe_distanceY + Feature::max_h;
                col = (m_generator() % (nCols - 2 * (m_probe_distanceX + Feature::max_w))) + m_probe_distanceX + Feature::max_w;
                intensity = image.at<uchar>(row, col);

//                qDebug() << id << " " << intensity << " sum: " << sum;
                if(sum < 25500)
                    break;
            }
            auto &px = m_pixelCloud.pixels1[last + k];
            px.id = id;
            px.label = label;
            px.position.x = col;
            px.position.y = row;
        }
        last += pxCount;
    }
}

void RandomDecisionTree::constructTree()
{
    constructRootNode();
    constructTreeDecisionNodes();
    SignalSenderInterface::instance().printsend("Updating nodes...");
    computeLeafHistograms();
    updateNodesWithNewDada();
    SignalSenderInterface::instance().printsend("Computing leaf Histogram...");
    computeLeafHistograms();
}

void RandomDecisionTree::constructRootNode()
{
    quint32 rootId = 0;
    m_nodes[rootId].id = rootId;
    m_nodes[rootId].end = m_pixelCloud.pixels1.size();
    m_nonLeafpxCount = m_pixelCloud.pixels1.size();
    m_statLog.logPxCount(m_nonLeafpxCount, 0);
    m_statLog.logLeafCount(0, 0);

    if(m_nodes[rootId].end >= TableLookUp::size)
        computeDivisionAt(rootId);
    else
        computeDivisionWithLookUpAt(rootId);

    ++m_featureFreq[m_nodes[rootId].ftrID];
    rearrange(rootId);
    m_pixelCloud.swap();
    calculateImpurity(0);
}

void RandomDecisionTree::constructTreeDecisionNodes()
{
    for (quint32 depth = 1; depth < m_height - 1; ++depth)
    {
        SignalSenderInterface::instance().printsend("Tree at depth " + QString::number(depth) + " being processed...");
        m_leafCount = 0;
        m_nonLeafpxCount = m_pixelCloud.pixels1.size();
        int level_nodes = 1ul << depth;          // Number of nodes at this level
        int tot_nodes   = (1ul << (depth + 1)) - 1; // Number of nodes at this and previous levels
        tbb::parallel_for(tot_nodes - level_nodes, tot_nodes, 1, [ = ](int nodeIndex)
        {
            processNode(nodeIndex);
            rearrange(nodeIndex);
        });
        m_pixelCloud.swap();
        calculateImpurity(depth);
        m_statLog.logPxCount(m_nonLeafpxCount, depth);
        m_statLog.logLeafCount(m_leafCount, depth);
        qApp->processEvents();
    }
}

void RandomDecisionTree::updateNodesWithNewDada()
{
    // For hist calc use different pxls
    m_pixelCloud.pixels1.clear();
    m_pixelCloud.pixels2.clear();
    m_params->pixelsPerImage = m_params->pixelsPerImage + 100;
    getSubSampleSingleFrame();

    rearrange(0);   // process root
    m_pixelCloud.swap();

    for (quint32 depth = 1; depth < m_height - 1; ++depth)
    {
        int level_nodes = 1ul << depth;          // Number of nodes at this level
        int tot_nodes   = (1ul << (depth + 1)) - 1; // Number of nodes at this and previous levels
        tbb::parallel_for(tot_nodes - level_nodes, tot_nodes, 1, [ = ](int nodeIndex)
        {
            auto mult                = (nodeIndex + 1) % 2; // 0 if left, 1 if right
            auto parentId            = (nodeIndex + 1) / 2 - 1;
            auto leftCount           = m_nodes[parentId].leftCount;
            auto rightCount          = m_nodes[parentId].end - m_nodes[parentId].start - leftCount;
            m_nodes[nodeIndex].start = m_nodes[parentId].start + mult * leftCount;
            m_nodes[nodeIndex].end   = m_nodes[parentId].end - ((mult + 1) % 2) * rightCount;
            rearrange(nodeIndex);
        });
        m_pixelCloud.swap();
    }
}

void RandomDecisionTree::computeLeafHistograms()
{
    auto tot_node_count = (1ul << m_height) - 1 ;
    auto leaf_node_count = 1ul << (m_height - 1) ;
    for (auto node_id = (tot_node_count - leaf_node_count); node_id < tot_node_count; ++node_id)
    {
        auto& parent = m_nodes[(node_id + 1) / 2 - 1];
        auto leftCount = parent.leftCount;
        auto rightCount = parent.end - parent.start - leftCount;
        auto mult = (node_id + 1) % 2; // 0 if left, 1 if right
        auto start = parent.start + mult * leftCount;
        auto end = parent.end - ((mult + 1) % 2) * rightCount;
        auto pxCount = end - start;


        if (pxCount == 0)
        {
            parent.tau = 1000*mult - 500; // Supress all nodes to right if left node is empty
            continue;
        }

        m_nodes[node_id].isLeaf = true;
        m_nodes[node_id].id = node_id;
        m_nodes[node_id].start = start;
        m_nodes[node_id].end = end;
        m_nodes[node_id].hist = computeHistogramNorm(start, end, m_params->labelCount);
    }

    m_pixelCloud.pixels1.clear();
    m_pixelCloud.pixels2.clear();
}


void RandomDecisionTree::computeDivisionAt(quint32 index)
{
    auto px_count = m_nodes[index].end - m_nodes[index].start;
    if (px_count == 0)
        return;
    auto maxItr             = m_params->maxIteration;
    auto nLabels            = m_params->labelCount;
    auto maxTau             = std::numeric_limits<qint16>::max();
    auto maxFeatureIndex    = std::numeric_limits<quint8>::min();
    auto maxGain            = std::numeric_limits<float>::min();

    cv::Point maxTeta1{}, maxTeta2{};
    QVector<quint32> leftHist(nLabels);
    QVector<quint32> rightHist(nLabels);

    auto totalFeatures = Feature::features.size();
    auto parentEntr = calculateEntropy(computeHistogram(m_nodes[index].start, m_nodes[index].end, nLabels));
    auto infoGain = 0.0f;
    auto itr = 0;

    while (itr < maxItr)
    {
        generateParams(index);

        for (quint8 feature = 0; feature < totalFeatures; ++feature)
        {
            m_nodes[index].ftrID = feature;
            computeHistograms(index, rightHist, leftHist);
            infoGain = parentEntr - calculateEntropy(leftHist) - calculateEntropy(rightHist);

            // Non-improving epoch :
            if (infoGain > maxGain)
            {
                maxTeta1 = m_nodes[index].teta1;
                maxTeta2 = m_nodes[index].teta2;
                maxTau   = m_nodes[index].tau;
                maxGain  = infoGain;
                maxFeatureIndex = feature;
                itr = -1 ;
            }
        }
        ++itr;
    }

    setParamsFor(maxFeatureIndex, maxTeta1, maxTeta2, index, maxTau);
}

void RandomDecisionTree::computeDivisionWithLookUpAt(quint32 index)
{
    auto px_count = m_nodes[index].end - m_nodes[index].start;
    if (px_count == 0)
        return;
    auto maxItr             = m_params->maxIteration;
    auto nLabels            = m_params->labelCount;
    auto maxTau             = std::numeric_limits<qint16>::max();
    auto maxFeatureIndex    = std::numeric_limits<qint8>::min();
    auto maxGain            = std::numeric_limits<float>::min();

    cv::Point maxTeta1{}, maxTeta2{};
    QVector<quint32> leftHist(nLabels);
    QVector<quint32> rightHist(nLabels);

    auto totalFeatures = Feature::features.size();
    auto parentEntr = calculateEntropyLookUp(computeHistogram(m_nodes[index].start, m_nodes[index].end, nLabels));
    auto infoGain = 0.0f;
    auto itr = 0;



    while (itr < maxItr)
    {
        generateParams(index);

        for (quint8 feature = 0; feature < totalFeatures; ++feature)
        {
            m_nodes[index].ftrID = feature;
            computeHistograms(index, rightHist, leftHist);
            infoGain = parentEntr - calculateEntropyLookUp(leftHist) - calculateEntropyLookUp(rightHist);

            // Non-improving epoch :
            if (infoGain > maxGain)
            {
                maxTeta1 = m_nodes[index].teta1;
                maxTeta2 = m_nodes[index].teta2;
                maxTau   = m_nodes[index].tau;
                maxGain  = infoGain;
                maxFeatureIndex = feature;
                itr = -1 ;
            }
        }
        ++itr;
    }

    setParamsFor(maxFeatureIndex, maxTeta1, maxTeta2, index, maxTau);
}

void RandomDecisionTree::rearrange(quint32 index)
{
    quint32 start = m_nodes[index].start;
    quint32 end = m_nodes[index].end;
    int dx = start;
    int dy = end - 1;
    int leftCount = 0;
    for (auto i = start; i < end; ++i)
    {
        auto &px = m_pixelCloud.pixels1[i];
        auto &img = m_DS->images[px.id];
        if (m_nodes[index].isLeft(px, img))
        {
            m_pixelCloud.pixels2[dx++] = m_pixelCloud.pixels1[i];
            leftCount++;
        }
        else
            m_pixelCloud.pixels2[dy--] = m_pixelCloud.pixels1[i];
    }
    m_nodes[index].leftCount = leftCount;
}


bool RandomDecisionTree::isPixelSizeConsistent()
{
    auto tot_node_count = (1 << m_height) - 1 ;
    auto leaf_node_count = 1 << (m_height - 1) ;
    auto decision_node_count = tot_node_count - leaf_node_count;
    auto nPixelsOnLeaves = 0;
    for (int nodeIndex = decision_node_count; nodeIndex < tot_node_count; ++nodeIndex)
        nPixelsOnLeaves += (m_nodes[nodeIndex].end - m_nodes[nodeIndex].start);
    return m_pixelCloud.pixels1.size() == nPixelsOnLeaves;
}
