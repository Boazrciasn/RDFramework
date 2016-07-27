#include "include/RandomDecisionTree.h"
#include "include/RandomDecisionForest.h"


// create the tree
RandomDecisionTree::RandomDecisionTree(rdf_ptr DF) : m_sDF(DF)
{
    //        rdfclock::time_point beginning = rdfclock::now();
    //        rdfclock::duration d = rdfclock::now()-beginning;
    std::random_device rd;
    generator = std::mt19937(rd());
    m_disProbTau = std::uniform_int_distribution<>(-127, 128);
    //generator = new std::mt19937(d.count());
}

RandomDecisionTree::RandomDecisionTree(RandomDecisionForest *DF) : m_DF(DF)
{
    //        rdfclock::time_point beginning = rdfclock::now();
    //        rdfclock::duration d = rdfclock::now()-beginning;
    std::random_device rd;
    generator = std::mt19937(rd());
    m_disProbTau = std::uniform_int_distribution<>(-127, 128);
    //generator = new std::mt19937(d.count());
}

void RandomDecisionTree::train()
{
    subSample();
    node_ptr root(new Node(1, false));
    root->m_tau = generateTau();
    generateTeta(root->m_teta1, m_probe_distanceX, m_probe_distanceY);
    generateTeta(root->m_teta2, m_probe_distanceX, m_probe_distanceY);
    m_nodes[0] = root;
    m_depth = 1;
    m_numOfLeaves = 0;
    constructTree(*root, m_pixelCloud);
    emit  m_DF->treeConstructed();
}

// create child nodes from nodes by tuning each node
void RandomDecisionTree::constructTree( Node &cur_node, PixelCloud &pixels)
{
    //float rootEntropy = calculateEntropyOfVector(pixels);
    int current_depth = log2(cur_node.m_id) + 1;
    if(current_depth > m_depth)
        m_depth = current_depth;
    //no more child construction
    // min etropy removed
    if( current_depth >= m_maxDepth || pixels.size() <= m_minLeafPixelCount )
    {
        m_nodes[cur_node.m_id - 1]->m_isLeaf = true;
        m_nodes[cur_node.m_id - 1]->m_hist = createHistogram(pixels,
                                                             m_DF->m_params.labelCount);
        ++m_numOfLeaves;
        return;
    }
    tuneParameters(pixels, cur_node);
    std::vector<pixel_ptr> left;
    std::vector<pixel_ptr> right;
    divide(m_DF->m_DS, pixels, left, right, cur_node);
    node_ptr leftChildNode(new Node(2 * cur_node.m_id, false));
    leftChildNode->m_tau = generateTau();
    generateTeta(leftChildNode->m_teta1, m_probe_distanceX, m_probe_distanceY);
    generateTeta(leftChildNode->m_teta2, m_probe_distanceX, m_probe_distanceY);
    m_nodes[leftChildNode->m_id - 1] = leftChildNode;
    constructTree(*leftChildNode, left);
    node_ptr rightChildNode(new Node(2 * cur_node.m_id + 1, false));
    rightChildNode->m_tau = generateTau();
    generateTeta(rightChildNode->m_teta1, m_probe_distanceX, m_probe_distanceY);
    generateTeta(rightChildNode->m_teta2, m_probe_distanceX, m_probe_distanceY);
    m_nodes[rightChildNode->m_id - 1] = rightChildNode;
    constructTree(*rightChildNode, right);
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
        imageinfo_ptr img_inf(new ImageInfo(label, sampleId++));
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
            pixel_ptr px(new Pixel(Coord(i, j), intensity, img_inf));
            m_pixelCloud.push_back(px);
        }
    }
}

// find best teta and taw parameters for the given node
void RandomDecisionTree::tuneParameters(PixelCloud &parentPixels, Node &parent)
{
    std::vector<pixel_ptr> left;
    std::vector<pixel_ptr> right;
    int maxTau = -250;
    Coord maxTeta1, maxTeta2;
    float maxGain = 0;
    int itr = 0;
    auto nLabels = m_DF->m_params.labelCount;
    while(itr < m_DF->m_params.maxIteration)
    {
        //printNode(parent);
        divide(m_DF->m_DS, parentPixels, left, right, parent);
        //qDebug() << "Left " << left.size() << "Right " << right.size();
        float leftChildEntr = calculateEntropyOfVector(left, nLabels);
        //qDebug() << "EntLeft" << leftChildEntr ;
        float rightChildEntr = calculateEntropyOfVector(right, nLabels);
        //qDebug() << "EntRight" << rightChildEntr ;
        int sizeLeft  = left.size();
        int sizeRight = right.size();
        float totalsize = parentPixels.size() ;
        float avgEntropyChild  = (sizeLeft / totalsize) * leftChildEntr;
        avgEntropyChild += (sizeRight / totalsize) * rightChildEntr;
        float parentEntr = calculateEntropyOfVector(parentPixels, nLabels);
        float infoGain = parentEntr - avgEntropyChild;
        // qDebug() << "InfoGain" << infoGain ;
        // Non-improving epoch :
        if(infoGain > maxGain)
        {
            maxTeta1 = parent.m_teta1;
            maxTeta2 = parent.m_teta2;
            maxTau   = parent.m_tau;
            maxGain  = infoGain;
            itr = 0 ;
        }
        else
        {
            ++itr;
        }
        //left.clear();
        //right.clear();
        generateTeta(parent.m_teta1, m_probe_distanceX, m_probe_distanceY);
        generateTeta(parent.m_teta2, m_probe_distanceX, m_probe_distanceY);
        parent.m_tau = generateTau();
    }
    parent.m_tau = maxTau;
    parent.m_teta1 = maxTeta1;
    parent.m_teta2 = maxTeta2;
}


bool RandomDecisionTree::isPixelSizeConsistent()
{
    auto nPixelsOnLeaves = 0u;
    for(node_ptr node : m_nodes)
        if(node->m_isLeaf)
            nPixelsOnLeaves += getTotalNumberOfPixels(node->m_hist);
    return m_pixelCloud.size() == nPixelsOnLeaves;
}

void RandomDecisionTree::toString()
{
    qDebug() << "Size  : " << m_nodes.size() ;
    qDebug() << "Depth : " << m_depth;
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
    for(auto pPixel : m_pixelCloud)
        printPixel(pPixel);
}

void RandomDecisionTree::printPixel(pixel_ptr px)
{
    qDebug() << "Pixel{ Coor("    << px->position.m_dy     << ","     <<
             px->position.m_dx
             << ") Label("        << px->imgInfo->m_label << ") Id(" <<
             px->imgInfo->m_sampleId
             << ") = "            << px->intensity << "}";
}

void RandomDecisionTree::printTree()
{
    qDebug() << "TREE {";
    for(auto node : m_nodes)
    {
        if(node != nullptr)
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
