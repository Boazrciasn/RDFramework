#include "RandomDecisionTree.h"
#include "RandomDecisionForest.h"

RandomDecisionTree::RandomDecisionTree(DataSet *DS, RDFParams *params) : m_DS(DS), m_params(params)
{
    //        rdfclock::time_point beginning = rdfclock::now();
    //        rdfclock::duration d = rdfclock::now()-beginning;
    std::random_device rd;
    generator = std::mt19937(rd());
    m_tauProbDistribution = std::uniform_int_distribution<>(-127, 128);
    //generator = new std::mt19937(d.count());
}



void RandomDecisionTree::train()
{
    initParams();
    initNodes(); // initNodes, it alocates space for nodes
    getSubSample();
    constructTree();
    emit treeConstructed();
}

void RandomDecisionTree::getSubSample()
{
    quint32 imgCount = m_DS->m_ImagesVector.size();
    quint32 pxPerImgCount = m_params->pixelsPerImage;

    m_pixelCloud.pixels1.resize(imgCount*pxPerImgCount);
    m_pixelCloud.pixels2.resize(imgCount*pxPerImgCount);


    for (quint32 id = 0; id < imgCount; ++id)
    {
        cv::Mat image;
        // Zero pad given image
        cv::copyMakeBorder(m_DS->m_ImagesVector[id],image,m_probe_distanceY, m_probe_distanceY,
                           m_probe_distanceX,m_probe_distanceX, cv::BORDER_CONSTANT);


        auto label  = m_DS->m_labels[id];
        int nRows = image.rows;
        int nCols = image.cols;

        for (int k = 0; k < m_params->pixelsPerImage; ++k)
        {
            int row = 0;
            int col = 0;
            quint8 intensity = 0 ;
            while (intensity == 0)
            {
                row = (rand() % (nRows - 2 * m_probe_distanceY)) + m_probe_distanceY;
                col = (rand() % (nCols - 2 * m_probe_distanceX)) + m_probe_distanceX;
                intensity = image.at<uchar>(row, col);
            }

            auto &px = m_pixelCloud.pixels1[id*pxPerImgCount + k];
            px.id = id;
            px.label = label;
            px.position.x = row;
            px.position.y = col;
        }
    }
}

void RandomDecisionTree::constructTree()
{
    constructRootNode();
    constructTreeDecisionNodes();
    computeLeafHistograms();
}

void RandomDecisionTree::constructRootNode()
{
    quint32 rootId = 0;
    m_nodes[rootId].id = rootId;
    m_nodes[rootId].end = m_pixelCloud.pixels1.size();
    m_nodes[rootId].tau = generateTau();
    generateTeta(m_nodes[rootId].teta1);
    generateTeta(m_nodes[rootId].teta2);
    computeDivisionAt(rootId);
}

void RandomDecisionTree::constructTreeDecisionNodes()
{
    for (quint32 depth = 1; depth < m_height-1; ++depth)
    {
        int level_nodes = 1 << depth;          // Number of nodes at this level
        int tot_nodes   = (1 << (depth + 1)) - 1; // Number of nodes at this and previous levels
        tbb::parallel_for(tot_nodes - level_nodes, tot_nodes, 1, [ = ](int nodeIndex)
        {
            processNode(nodeIndex);
            rearrange(nodeIndex);
        });
        m_pixelCloud.swap();
    }
}

void RandomDecisionTree::computeLeafHistograms()
{
    auto tot_node_count = (1 << m_height) - 1 ;
    auto leaf_node_count = 1 << (m_height - 1) ;
    for (int node_id = (tot_node_count - leaf_node_count); node_id < tot_node_count; ++node_id)
    {
        Node parent = m_nodes[(node_id + 1) / 2];
        quint32 leftCount = parent.leftCount;
        quint32 rightCount = parent.end - parent.start - leftCount;
        int mult = (node_id + 1) % 2; // 0 if left, 1 if right
        auto x = parent.start + mult * leftCount;
        auto y = parent.end - ((mult + 1) % 2) * rightCount;
        m_nodes[node_id].hist = computeHistogram(x, y, m_params->labelCount);
    }
}

void RandomDecisionTree::computeDivisionAt(quint32 index)
{
    // find best teta and taw parameters for the given node
    int px_count = m_nodes[index].end - m_nodes[index].start;
    if (px_count == 0)
        return;
    auto maxItr = m_params->maxIteration;
    auto nLabels = m_params->labelCount;
    int maxTau = -250;
    float maxGain = 0;
    cv::Point maxTeta1, maxTeta2;
    int itr = 0;
    cv::Mat_<float> leftHist(1, nLabels);
    cv::Mat_<float> rightHist(1, nLabels);
    float leftChildEntr, rightChildEntr;
    float avgEntropyChild, parentEntr;
    float infoGain;
    while (itr < maxItr)
    {
        leftHist.setTo(0.0f);
        rightHist.setTo(0.0f);
        int sizeLeft  = 0;
        int sizeRight = 0;
        auto end = m_nodes[index].end;
        for (auto i = m_nodes[index].start; i < end; ++i)
        {
            auto &px = m_pixelCloud.pixels1[i];
            auto &img = m_DS->m_ImagesVector[px.id];
            if (isLeft(px, m_nodes[index], img))
            {
                ++leftHist.at<float>(px.label);
                sizeLeft++;
            }
            else
            {
                ++rightHist.at<float>(px.label);
                sizeRight++;
            }
        }
        leftChildEntr = calculateEntropy(leftHist);
        rightChildEntr = calculateEntropy(rightHist);
        avgEntropyChild  = (sizeLeft / px_count) * leftChildEntr;
        avgEntropyChild += (sizeRight / px_count) * rightChildEntr;
        parentEntr = calculateEntropy(computeHistogram(m_nodes[index].start, m_nodes[index].end, nLabels));
        infoGain = parentEntr - avgEntropyChild;
        // Non-improving epoch :
        if (infoGain > maxGain)
        {
            maxTeta1 = m_nodes[index].teta1;
            maxTeta2 = m_nodes[index].teta2;
            maxTau   = m_nodes[index].tau;
            maxGain  = infoGain;
            itr = 0 ;
        }
        else
            ++itr;
        generateTeta(m_nodes[index].teta1);
        generateTeta(m_nodes[index].teta2);
        m_nodes[index].tau = generateTau();
    }
    m_nodes[index].tau = maxTau;
    m_nodes[index].teta1 = maxTeta1;
    m_nodes[index].teta2 = maxTeta2;
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
        auto &img = m_DS->m_ImagesVector[px.id]; // TODO: might be too time consuming
        if (isLeft(px, m_nodes[index], img))
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
        nPixelsOnLeaves += getTotalNumberOfPixels(m_nodes[nodeIndex].hist);
    return m_pixelCloud.pixels1.size() == nPixelsOnLeaves;
}

void RandomDecisionTree::toString()
{
    // TODO: convert or delete
    //    qDebug() << "Size  : " << m_nodes.size() ;
    //    qDebug() << "Depth : " << m_height;
    //    qDebug() << "Leaves: " << m_numOfLeaves;
    //    int count = 0 ;
    //    for (Node node : m_nodes)
    //    {
    //        if(node->m_isLeaf)
    //        {
    //            ++count;
    //            printHistogram(node->m_hist);
    //        }
    //    }
    //    qDebug() << "Number of leaves : " << count ;
}

void RandomDecisionTree::printPixelCloud()
{
    for (auto &pPixel : m_pixelCloud.pixels1)
        printPixel(pPixel);
}

void RandomDecisionTree::printPixel(Pixel &px)
{
    qDebug() << "Pixel{ Coor("    << px.position.y     << ","     <<
             px.position.x
             << ") Label("        << px.label << ") Id(" <<
             px.id
             << ") }";
}

void RandomDecisionTree::printTree()
{
    qDebug() << "TREE {";
    for (auto &node : m_nodes)
        printNode(node);
    qDebug() << "}";
}


void RandomDecisionTree::printNode(Node &node)
{
    qDebug() << "NODE {"
             << " Id:"   << node.id
             << " Tau: " << node.tau
             << " Q1 {" << node.teta1.y << "," << node.teta1.x << "}"
             << " Q2 {" << node.teta2.y << "," << node.teta2.x << "}";
    printHistogram(node.hist);
    qDebug() << "}";
}
