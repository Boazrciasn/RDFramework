#include "precompiled.h"

#include "include/RandomDecisionForest.h"
#include "include/Reader.h"
#include "include/Util.h"

// create the tree
void RandomDecisionTree::train(PixelCloud &pixelCloud)
{
    subSample();
    Node *root = new Node(1, false);
    root->m_tau = generateTau();
    generateTeta(root->m_teta1, m_probe_distanceX, m_probe_distanceY);
    generateTeta(root->m_teta2, m_probe_distanceX, m_probe_distanceY);
    m_nodes[root->m_id-1] = root;
    m_depth=1;
    m_numOfLeaves = 0;
    constructTree(*root, pixelCloud);
    emit treeConstructed();
}

// create child nodes from nodes by tuning each node
void RandomDecisionTree::constructTree( Node& root, std::vector<Pixel*>& pixels)
{
    //float rootEntropy = calculateEntropyOfVector(pixels);
    int current_depth = log2(root.m_id) + 1;
    if(current_depth > m_depth)
        m_depth = current_depth;
    //no more child construction
    // min etropy removed
    if( current_depth >= m_maxDepth || pixels.size() <= m_minLeafPixelCount )
    {
        m_nodes[root.m_id-1]->m_isLeaf = true;
        m_nodes[root.m_id-1]->m_hist = createHistogram(pixels, m_DF->m_labelCount);
        ++m_numOfLeaves;
        return;
    }

    tuneParameters(pixels, root);
    std::vector<Pixel*> left;
    std::vector<Pixel*> right;
    divide(m_DF->m_DS, pixels, left, right, root);


    //if(left.size()>0)
    //{
        Node *leftChildNode = new Node(2*root.m_id,false);
        leftChildNode->m_tau = generateTau();
        generateTeta(leftChildNode->m_teta1, m_probe_distanceX, m_probe_distanceY);
        generateTeta(leftChildNode->m_teta2, m_probe_distanceX, m_probe_distanceY);
        m_nodes[leftChildNode->m_id-1] = leftChildNode;
        constructTree(*leftChildNode,left);
    //}

    //if(right.size()>0)
    //{
        Node *rightChildNode = new Node(2*root.m_id+1, false);
        rightChildNode->m_tau = generateTau();
        generateTeta(rightChildNode->m_teta1, m_probe_distanceX, m_probe_distanceY);
        generateTeta(rightChildNode->m_teta2, m_probe_distanceX, m_probe_distanceY);
        m_nodes[rightChildNode->m_id-1] = rightChildNode;
        constructTree(*rightChildNode,right);
    //}
}

void RandomDecisionTree::subSample()
{

    int sampleId=0;
    m_DF->m_pixelCloud.clear();

    for(auto &image : m_DF->m_DS.m_trainImagesVector)
    {
        auto label = m_DF->m_labels[sampleId];
        ImageInfo *img_inf = new ImageInfo(label, sampleId++);

        // draw perImagePixel pixels from image
        // bootstrap, subsample
        for(int k=0; k<m_DF->m_pixelsPerImage; ++k)
        {
            int i = (rand() % (image.rows-2*m_probe_distanceY)) + m_probe_distanceY;
            int j = (rand() % (image.cols-2*m_probe_distanceX)) + m_probe_distanceX;
            auto intensity = image.at<uchar>(i,j);
            auto *px = new Pixel(Coord(i,j),intensity,img_inf);

            m_DF->m_pixelCloud.push_back(px);
        }
    }
//    qDebug()<<"pixel Cloud : " << pixelCloud.size();

}

// find best teta and taw parameters for the given node
void RandomDecisionTree::tuneParameters(std::vector<Pixel*>& parentPixels, Node& parent)
{
    std::vector<Pixel*> left;
    std::vector<Pixel*> right;
    int maxTau;
    Coord maxTeta1,maxTeta2;
    float maxGain=0;
    int itr=0;

    auto nLabels = m_DF->m_labelCount;
    while(itr < m_DF->m_maxIterationForDivision)
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
        float avgEntropyChild  = (sizeLeft/totalsize) * leftChildEntr;
              avgEntropyChild += (sizeRight/totalsize)* rightChildEntr;

        float parentEntr = calculateEntropyOfVector(parentPixels, nLabels);
        float infoGain = parentEntr - avgEntropyChild;
        // qDebug() << "InfoGain" << infoGain ;

        // Non-improving epoch :
        if(infoGain > maxGain)
        {
            maxTeta1 = parent.m_teta1;
            maxTeta2 = parent.m_teta2;
            maxTau   = parent.m_tau;
            maxGain = infoGain;
            itr = 0 ;
        }
        else
        {
            ++itr;
        }

        left.clear();
        right.clear();
        generateTeta(parent.m_teta1, m_probe_distanceX, m_probe_distanceY);
        generateTeta(parent.m_teta2, m_probe_distanceX, m_probe_distanceY);
        parent.m_tau = generateTau();
    }

    parent.m_tau = maxTau;
    parent.m_teta1 = maxTeta1;
    parent.m_teta2 = maxTeta2;
}

//padded image index must be provided
cv::Mat RandomDecisionForest::classify(int index)
{
    cv::Mat test_image = m_DS.m_testImagesVector[index];
    int n_rows=test_image.rows;
    int n_cols=test_image.cols;
    //typecheck
    cv::Mat res_image = cv::Mat(n_rows-2*m_probe_distanceY, n_cols-2*m_probe_distanceX, test_image.type());
    ImageInfo* img_Info = new ImageInfo(' ', index);

    for(int r=m_probe_distanceY; r<n_rows-m_probe_distanceY; ++r)
    {
        for(int c=m_probe_distanceX; c<n_cols-m_probe_distanceX; ++c)
        {
            auto intensity = test_image.at<uchar>(r,c);
            auto *px = new Pixel(Coord(r,c),intensity,img_Info);
            cv::Mat probHist = cv::Mat::zeros(1, m_labelCount, cv::DataType<float>::type);
            auto nForest = m_forest.size();
            for(unsigned int i=0; i<nForest; ++i)
            {
                Node *leaf = m_forest[i]->getLeafNode(m_DS, px, 0);
                probHist += leaf->m_hist;
            }
            // Type check of label
            auto label = getMaxLikelihoodIndex(probHist);
            res_image.at<uchar>(r-m_probe_distanceY,c-m_probe_distanceX) = label;
            probHist.release();
        }
    }

    return res_image;
}

void RandomDecisionForest::test()
{
    int size = m_DS.m_testImagesVector.size();
    qDebug() << "Number of Test images:" << QString::number(size);

    for(auto i=0; i<size; ++i)
    {
        cv::Mat votes = cv::Mat::zeros(1, m_labelCount, CV_32FC1);
//        cv::Mat original = unpad(m_testImagesVector[i]);
        //qDebug() << QString::number(i);
        //qDebug() << QString::number(i) << "labeled";
//        cv::Mat colored_img = colorCoder(prediction,original);
//        cv::imshow("Colored Image",colored_img );
//        cv::waitKey();
        //qDebug()<<"classify : " << QString::number(i);
        cv::Mat label_image = classify(i);
        //qDebug()<<" gettin votes : ";

        if(votes.cols != m_labelCount)
        {
            std::cerr<<"Assertion Failed BRO!  Number of labels mismatch"<< std::endl;
            return;
        }
        getImageLabelVotes(label_image, votes);
        std::cout<<votes<<std::endl;
        int label = getMaxLikelihoodIndex(votes);
        //qDebug()<<" LABELED : "<< char('a' + label);
        emit classifiedImageAs(i+1, char('a' + label));
        votes.release();
    }
}


bool RandomDecisionTree::isPixelSizeConsistent()
{
    auto nPixelsOnLeaves=0u;
    for(Node *node : m_nodes)
        if(node->m_isLeaf)
            nPixelsOnLeaves += getTotalNumberOfPixels(node->m_hist);

    return m_DF->m_pixelCloud.size() == nPixelsOnLeaves;
}

void RandomDecisionTree::toString()
{
    qDebug() << "Size  : " << m_nodes.size() ;
    qDebug() << "Depth : " << m_depth;
    qDebug() << "Leaves: " << m_numOfLeaves;
    int count = 0 ;
    for (auto *node : m_nodes)
    {
        if(node->m_isLeaf)
        {
            ++count;
            printHistogram(node->m_hist);
        }
    }
    qDebug()<< "Number of leaves : " << count ;
}

// histogram normalize ?
// getLeafNode and Test  needs rework
// given the directory of the all samples
// read subsampled part of the images into pixel cloud
void RandomDecisionForest::readTrainingImageFiles()
{
    m_dir = m_trainpath;
    std::vector<QString> fNames;
    auto *reader = new Reader();
    reader->findImages(m_dir, "", fNames);
    m_numOfLetters = fNames.size();
    qDebug()<<"NO OF LETTERS : "<< m_numOfLetters;
    for (auto filePath : fNames)
    {
        QString fileName = Util::fileNameWithoutPath(filePath);
        //each directory contains lastsession.txt
        if(!fileName.contains("_"))
            continue;

        filePath += "/" + fileName + ".jpg";
        cv::Mat image = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

        //pad image and save to vector
        cv::copyMakeBorder(image, image, m_probe_distanceY, m_probe_distanceY, m_probe_distanceX, m_probe_distanceX, cv::BORDER_CONSTANT);
        m_DS.m_trainImagesVector.push_back(image);

        //sample image info : label : a , Id : index of in the image vector of the image.
        QString letter = fileName.split("_")[0];
        // store label of the image in m_labels vector
        m_labels.push_back(letter[0].toLatin1());
    }
    qDebug()<<"No of IMAGES : " << m_DS.m_trainImagesVector.size() << " NO of Fnames" <<m_numOfLetters<< "Images Vector Size : "<< m_DS.m_trainImagesVector.size();
    fNames.clear();
}

void RandomDecisionForest::readTestImageFiles()
{
    m_DS.m_testImagesVector.clear();
    m_dir = m_testpath;
    std::vector<QString> fNames;
    auto *reader = new Reader();
    reader->findImages(m_dir, "", fNames);
    for (auto filePath : fNames)
    {
        QString fileName = Util::fileNameWithoutPath(filePath);
        //qDebug() << fileName ;
        //each directory contains lastsession.txt
        if(!fileName.contains("_"))
            continue;

        filePath += "/" + fileName + ".jpg";
        cv::Mat image = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

        //pad image and save to vector
        cv::copyMakeBorder(image, image, m_probe_distanceY, m_probe_distanceY, m_probe_distanceX, m_probe_distanceX, cv::BORDER_CONSTANT);
        m_DS.m_testImagesVector.push_back(image);
    }
    qDebug()<<"No of test IMAGES : " << m_DS.m_testImagesVector.size();
    fNames.clear();
}

//FOR TEST PURPOSES ONLY : given the image path, fills the vector with in the pixels of the image, img_Info : label of  test image & id of the image inside vector(optional)
void RandomDecisionForest::imageToPixels(std::vector<Pixel*> &res,const cv::Mat &image ,ImageInfo* img_inf )
{
    for(int i =0; i<image.rows;i++)
    {
        for(int j =0; j<image.cols;j++)
        {
            auto intensity = image.at<uchar>(i,j);
            Pixel* px = new Pixel(Coord(i,j),intensity,img_inf);
            res.push_back(px);
        }
    }
}

void RandomDecisionForest::printPixelCloud()
{
    for(auto pPixel : m_pixelCloud)
        printPixel(pPixel);
}

void RandomDecisionForest::printPixel(Pixel* px)
{
    qDebug() << "Pixel{ Coor("    << px->position.m_dy     << ","     << px->position.m_dx
             << ") Label("        << px->imgInfo->label << ") Id(" << px->imgInfo->sampleId
             << ") = "            << px->intensity << "}";
}

// given a letter returns its index on label histogram
// checks if a pixel will travel to the left of a given node
// divide pixel vector into 2 parts according to the parameters of parent node
// returns the image the pixel belongs to
cv::Mat RandomDecisionForest::getPixelImage(Pixel* px)
{
    QString path = m_dir + "/"+ px->imgInfo->label + "/" + px->imgInfo->label + "_"
            + QString::number(px->imgInfo->sampleId)  + ".jpg";
    //qDebug()<<"IMAGE :"<< path;
    return cv::imread(path.toStdString());
}

cv::Mat RandomDecisionForest::colorCoder(const cv::Mat &labelImage, const cv::Mat &InputImage)
{
    cv::Mat color(InputImage.rows, InputImage.cols, CV_8UC1);
    //create color code Mat accoridng to of each pixel.
    for(int i =0; i<InputImage.rows;i++)
    {
        for(int j =0; j<InputImage.cols;j++)
        {
            auto label = labelImage.at<double>(i,j);
            color.at<double>(i,j) = (label/m_labelCount)*255;
        }
    }
    //create 3 channel image with 2 channels with same intensity values of cv::Mat color :

    std::vector<cv::Mat> Image_channels(3);
    Image_channels.at(0) = InputImage;
    Image_channels.at(1) = color;
    Image_channels.at(2) = color;
    cv::Mat codedImage;

    cv::merge(Image_channels,codedImage);

    return codedImage;
}

void RandomDecisionForest::setTrainPath(QString path)
{
    this->m_trainpath = path;
}

void RandomDecisionForest::setTestPath(QString path)
{
    this->m_testpath = path;
}

void RandomDecisionForest::trainForest()
{
    qDebug()<<"no of trees : " << m_no_of_trees;
    for (int i = 0; i < m_no_of_trees; ++i)
    {
        qDebug()<< "Tree number " << QString::number(i+1) << "is being trained" ;

        auto *trainedRDT = new RandomDecisionTree(this);
        trainedRDT->train(m_pixelCloud);
        m_forest.push_back(trainedRDT);
    }
    qDebug()<< "Forest Size : " << m_forest.size();
}


int RandomDecisionForest::pixelCloudSize()
{
    return m_pixelCloud.size();
}

void RandomDecisionForest::printNode(Node& node){
    QString res = "NODE {";
    qDebug() << "Id:"   << node.m_id
             << "Taw: " << node.m_tau
             << "Q1{" << node.m_teta1.m_dy << "," << node.m_teta1.m_dx
             << "Q2{" << node.m_teta2.m_dy << "," << node.m_teta2.m_dx;
    printHistogram(node.m_hist);
    qDebug() << "}";
}
