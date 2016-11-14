#include "RandomDecisionTree.h"
#include "RandomDecisionForest.h"

RandomDecisionTree::RandomDecisionTree(RandomDecisionForest *DF) : m_DF(DF)
{
    //        rdfclock::time_point beginning = rdfclock::now();
    //        rdfclock::duration d = rdfclock::now()-beginning;
    std::random_device rd;
    generator = std::mt19937(rd());
    m_disProbTau = std::uniform_int_distribution<>(-127, 128);
    //generator = new std::mt19937(d.count());
}

void RandomDecisionTree::initNodes()
{
    // Here we are assuming that setMaxDepth has been cales
    // which alocates space for all nodes
    auto tot_node_count = (1 << m_maxDepth) - 1 ;
    auto leaf_node_count = 1 << (m_maxDepth-1) ;
    auto decision_node_count = tot_node_count - leaf_node_count;

    // Decision node
    for (int node_id = 0; node_id < decision_node_count; ++node_id)
        m_nodes[node_id] = node_ptr(new Node(node_id,false));

    // Leaf Nodes
    for (int node_id = decision_node_count; node_id < tot_node_count; ++node_id)
        m_nodes[node_id] = node_ptr(new Node(node_id,true));
}

void RandomDecisionTree::subSample()
{
    int sampleId = 0;
    std::vector<QString> trainingTextFiles;
    Reader::readTextFiles(m_DF->m_params.trainAnnotationsDir, trainingTextFiles);
    for(auto fname : trainingTextFiles)
    {
        QFile currAnnotateFile(fname);
        if(!currAnnotateFile.open(QIODevice::ReadOnly))
            std::cout <<
                      "failed to open file! \n";
        while(!currAnnotateFile.atEnd())
        {
            QString line = currAnnotateFile.readLine();
            QStringList currImage = line.split(' ');
            int imageId = currImage[0].split('.')[0].toInt();
            if (!m_DF->m_DS.m_TrainHashTable.contains(imageId))
            {
                QString imageFullPath  = m_DF->m_dir + "/" + currImage[0];
                cv::Mat image = cv::imread(imageFullPath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
                cv::copyMakeBorder(image, image, m_DF->m_params.probDistY, m_DF->m_params.probDistY, m_DF->m_params.probDistX,
                                   m_DF->m_params.probDistX, cv::BORDER_CONSTANT);
                m_DF->m_DS.m_TrainHashTable.insert(imageId, image);
            }
            else
            {
            }
        }
    }
    for(auto &image : m_DF->m_DS.m_trainImagesVector)
    {
        auto label = m_DF->m_DS.m_trainlabels[sampleId];
        auto id = sampleId++;
        int nRows = image.rows;
        int nCols = image.cols;
        for(int k = 0; k < m_DF->m_params.pixelsPerImage; ++k)
        {
            int i;
            int j;
            quint8 intensity = 0 ;
            while(intensity == 0)
            {
                i = (rand() % (nRows - 2 * m_probe_distanceY)) + m_probe_distanceY;
                j = (rand() % (nCols - 2 * m_probe_distanceX)) + m_probe_distanceX;
                intensity = image.at<uchar>(i, j);
            }
            pixel_ptr px(new Pixel(Coord(i, j), intensity, id,label));
            m_pixelCloud.pixels.push_back(px);
        }
    }
}

void RandomDecisionTree::constructTreeAtDepth(int height)
{
    // We can make node computation concurent at each level because nodes
    // at the same level are independent of eachother


    // TODO: base case is when we reach leaf nodes (when we reach m_maxDepth)
    // TODO: add if base case compute hist for leafs

    int level_nodes = 1<<height;            // Number of nodes at this level
    int tot_nodes   = 1<<(height+1) - 1;    // Number of nodes at this and previous levels

    // TODO: make it concurent_for
    for (int nodeIndex = (tot_nodes-level_nodes); nodeIndex < tot_nodes; ++nodeIndex) {
        int parentIndex = (nodeIndex+1)/2;
        node_ptr parent = m_nodes[parentIndex];
        processNode(m_nodes[nodeIndex], parent->m_dataRange, parent->m_leftCount);
    }

    constructTreeAtDepth(++m_height);
}

void RandomDecisionTree::computeLeafHistograms()
{
    auto tot_node_count = (1 << m_maxDepth) - 1 ;
    auto leaf_node_count = 1 << (m_maxDepth-1) ;

    for (int node_id = (tot_node_count-leaf_node_count); node_id < tot_node_count; ++node_id)
    {
        node_ptr parent = m_nodes[(node_id+1)/2];
        quint32 leftCount = parent->m_leftCount;
        quint32 rightCount = parent->m_dataRange.m_dy - parent->m_dataRange.m_dx - leftCount;

        int mult = (node_id+1)%2; // 0 if left, 1 if right
        auto x = parent->m_dataRange.m_dx + mult*leftCount;
        auto y = parent->m_dataRange.m_dy - ((mult+1)%2)*rightCount;
        Coord range(x,y);
        m_nodes[node_id]->m_hist = computeHistogram(range, m_DF->m_params.labelCount);
    }
}

void RandomDecisionTree::constructTree()
{
    Coord dataRange(0,m_pixelCloud.pixels.size()); // [... , ...) because it is inclusive
    processNode(m_nodes[0], dataRange, 0); // construct root
    constructTreeAtDepth(++m_height); // m_height is 0 initily at root
    computeLeafHistograms();
}

void RandomDecisionTree::train()
{
    initNodes();
    subSample();
    constructTree();
    emit  m_DF->treeConstructed();
}


// find best teta and taw parameters for the given node
void RandomDecisionTree::computeDivisionAt(Node &node)
{
    int px_count = node.m_dataRange.m_dy - node.m_dataRange.m_dx;
    if(px_count == 0) return;

    QVector<bool> srtVals(px_count);

    auto maxItr = m_DF->m_params.maxIteration;
    auto nLabels = m_DF->m_params.labelCount;

    int maxTau = -250;
    float maxGain = 0;
    Coord maxTeta1, maxTeta2;
    int itr = 0;

    while(itr < maxItr)
    {
        divide(m_DF->m_DS, srtVals, node);

        auto sizeLeft  = node.m_leftCount;
        auto sizeRight = node.m_dataRange.m_dy - node.m_dataRange.m_dx - sizeLeft;
        qDebug() << "Left: " << sizeLeft << ", Right: " << sizeRight;

        Coord lCrd(node.m_dataRange.m_dx,node.m_dataRange.m_dx+sizeLeft);
        Coord rCrd(lCrd.m_dy,node.m_dataRange.m_dy);
        float leftChildEntr = calculateEntropy(computeHistogram(lCrd ,nLabels));
//        qDebug() << "EntLeft" << leftChildEntr ;
        float rightChildEntr = calculateEntropy(computeHistogram(rCrd ,nLabels));
//        qDebug() << "EntRight" << rightChildEntr ;

        float avgEntropyChild  = (sizeLeft / px_count) * leftChildEntr;
        avgEntropyChild += (sizeRight / px_count) * rightChildEntr;
        float parentEntr = calculateEntropy(computeHistogram(node.m_dataRange, nLabels));
        float infoGain = parentEntr - avgEntropyChild;
        // qDebug() << "InfoGain" << infoGain ;


        // Non-improving epoch :
        if(infoGain > maxGain)
        {
            maxTeta1 = node.m_teta1;
            maxTeta2 = node.m_teta2;
            maxTau   = node.m_tau;
            maxGain  = infoGain;
            itr = 0 ;
        }
        else
        {
            ++itr;
        }

        generateTeta(node.m_teta1, m_probe_distanceX, m_probe_distanceY);
        generateTeta(node.m_teta2, m_probe_distanceX, m_probe_distanceY);
        node.m_tau = generateTau();
    }

    node.m_tau = maxTau;
    node.m_teta1 = maxTeta1;
    node.m_teta2 = maxTeta2;
}


bool RandomDecisionTree::isPixelSizeConsistent()
{
    auto tot_node_count = (1 << m_maxDepth) - 1 ;
    auto leaf_node_count = 1 << (m_maxDepth-1) ;
    auto decision_node_count = tot_node_count - leaf_node_count;

    auto nPixelsOnLeaves = 0u;

    for (int nodeIndex = decision_node_count; nodeIndex < tot_node_count; ++nodeIndex)
        nPixelsOnLeaves += getTotalNumberOfPixels(m_nodes[nodeIndex]->m_hist);

    return m_pixelCloud.pixels.size() == nPixelsOnLeaves;
}

void RandomDecisionTree::toString()
{
    qDebug() << "Size  : " << m_nodes.size() ;
    qDebug() << "Depth : " << m_height;
    qDebug() << "Leaves: " << m_numOfLeaves;
    int count = 0 ;
    for (node_ptr node : m_nodes)
    {
        if(node->m_isLeaf)
        {
            ++count;
            printHistogram(node->m_hist);
        }
    }
    qDebug() << "Number of leaves : " << count ;
}

void RandomDecisionTree::printPixelCloud()
{
    for(auto pPixel : m_pixelCloud.pixels)
        printPixel(pPixel);
}

void RandomDecisionTree::printPixel(pixel_ptr px)
{
    qDebug() << "Pixel{ Coor("    << px->position.m_dy     << ","     <<
             px->position.m_dx
             << ") Label("        << px->sampleLabel << ") Id(" <<
             px->sampleId
             << ") = "            << px->intensity << "}";
}

void RandomDecisionTree::printTree()
{
    qDebug() << "TREE {";
    for(auto node : m_nodes)
    {
        if(node)
            printNode(*node);
    }
    qDebug() << "}";
}


void RandomDecisionTree::printNode(Node &node)
{
    qDebug() << "NODE {"
             << " Id:"   << node.m_id
             << " Tau: " << node.m_tau
             << " Q1 {" << node.m_teta1.m_dy << "," << node.m_teta1.m_dx << "}"
             << " Q2 {" << node.m_teta2.m_dy << "," << node.m_teta2.m_dx << "}";
    printHistogram(node.m_hist);
    qDebug() << "}";
}
