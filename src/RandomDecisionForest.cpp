#include <QFileDialog>
#include <QImage>
#include <QDialog>
#include <QObject>

#include "include/RandomDecisionForest.h"
#include "include/Reader.h"
#include <opencv2/highgui/highgui.hpp>


// histogram normalize ?
// getLeafNode and Test  needs rework
// given the directory of the all samples
// read subsampled part of the images into pixel cloud
void RandomDecisionForest::readTrainingImageFiles(){

    m_dir = m_trainpath;
    std::vector<QString> fNames;
    auto *reader = new Reader();
    reader->findImages(m_dir, "", fNames);
    m_numOfLetters = fNames.size();
    qDebug()<<"NO OF LETTERS : "<< m_numOfLetters;
    for (auto it = std::begin(fNames) ; it != std::end(fNames); ++it)
    {
        QString filePath = *it;
        QString fileName = Util::fileNameWithoutPath(filePath);
        //each directory contains lastsession.txt
        if(!fileName.contains("_"))
            continue;

        filePath += "/" + fileName + ".jpg";
        cv::Mat image = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

        //pad image and save to vector
        cv::copyMakeBorder(image, image, m_probe_distanceY, m_probe_distanceY, m_probe_distanceX, m_probe_distanceX, cv::BORDER_CONSTANT);
        m_trainImagesVector.push_back(image);

        //sample image info : label : a , Id : index of in the image vector of the image.
        QString letter = fileName.split("_")[0];
        // store label of the image in m_labels vector
        m_labels.push_back(letter[0].toLatin1());
    }
    qDebug()<<"No of IMAGES : " << m_trainImagesVector.size() << " NO of Fnames" <<m_numOfLetters<< "Images Vector Size : "<< m_trainImagesVector.size();
    fNames.clear();
}

void RandomDecisionForest::readTestImageFiles(){

    m_testImagesVector.clear();
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
        m_testImagesVector.push_back(image);
    }
    qDebug()<<"No of test IMAGES : " << m_testImagesVector.size();
    fNames.clear();
}

void RandomDecisionForest::subSample(){

    int sampleId=0;
    m_pixelCloud.clear();

    for (auto it = std::begin(m_trainImagesVector) ; it != std::end(m_trainImagesVector); ++it)
    {
        auto image = *it;
        auto label = m_labels[sampleId];
        ImageInfo *img_inf = new ImageInfo(label,sampleId++);

        // draw perImagePixel pixels from image
        // bootstrap, subsample
        for(int k=0;k<m_pixelsPerImage; ++k)
        {
            int i = (rand() % (image.rows-2*m_probe_distanceY)) + m_probe_distanceY;
            int j = (rand() % (image.cols-2*m_probe_distanceX)) + m_probe_distanceX;
            auto intensity = image.at<uchar>(i,j);
            auto *px = new Pixel(Coord(i,j),intensity,img_inf);

            m_pixelCloud.push_back(px);
        }
    }
//    qDebug()<<"pixel Cloud : " << pixelCloud.size();

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

// create the tree
void RandomDecisionForest::trainTree()
{
    subSample();
    Node *root = new Node(1,false);
    root->m_tau = generateTau();
    generateTeta(root->m_teta1);
    generateTeta(root->m_teta2);
    m_tempTree[root->m_id-1] = root;
    m_depth=1;
    m_numOfLeaves = 0 ;
    constructTree(*root,m_pixelCloud);
    qDebug() << "TREE CONSTRUCTED: \n Leaf Count:" << m_numOfLeaves << ". " << m_numOfLetters<< "Samples Used";
    //qDebug() << "Pixel Sizes Are" << (pixelSizesConsistent() ? "Consistent" : "Not Consistent") ;
}

bool RandomDecisionForest::pixelSizesConsistent()
{
    auto nPixelsOnLeaves=0u;
    for(Node *node : m_tempTree)
    {
        if(node->m_isLeaf)
            nPixelsOnLeaves += getTotalNumberOfPixels(node->m_hist);
    }
    return m_pixelCloud.size() == nPixelsOnLeaves;
}

//padded image index must be provided
cv::Mat RandomDecisionForest::classify(int index)
{
    cv::Mat test_image = m_testImagesVector[index];
    int n_rows=test_image.rows;
    int n_cols=test_image.cols;
    //typecheck
    qDebug() << "A";
    cv::Mat res_image = cv::Mat(n_rows-2*m_probe_distanceY, n_cols-2*m_probe_distanceX, test_image.type());
    ImageInfo* img_Info = new ImageInfo(' ', index);

    qDebug() << "B";

    for(int r=m_probe_distanceY; r<n_rows-m_probe_distanceY; ++r)
    {
        for(int c=m_probe_distanceX; c<n_cols-m_probe_distanceX; ++c)
        {
            //qDebug() << QString::number(r) << "," << QString::number(c) ;
            auto intensity = test_image.at<uchar>(r,c);
            auto *px = new Pixel(Coord(r,c),intensity,img_Info);
            cv::Mat probHist = cv::Mat::zeros(1,m_labelCount,cv::DataType<float>::type);
            //qDebug() << "C";
            for(unsigned int i=0; i<m_forest.size(); ++i)
            {
               // qDebug() << "D" << QString::number(i+1);
                Node *leaf = getLeafNode(px, 0, m_forest[i].m_tree);
               // qDebug() << "DD" << QString::number(i+1);
                probHist += leaf->m_hist;
            }
            // Type check of label
            //printHistogram(probHist);
            auto label = getMaxLikelihoodIndex(probHist);
            res_image.at<uchar>(r-m_probe_distanceY,c-m_probe_distanceX) = label;
            probHist.release();
        }
    }

    return res_image;
}

void RandomDecisionForest::test()
{
    int size = m_testImagesVector.size();
    qDebug() << "Number of Test images:" << QString::number(size);

    //cv::Mat votes_result = cv::Mat::zeros(1, m_labelCount, CV_32FC1);
    for(auto i=0; i<size; ++i)
    {
        cv::Mat votes = cv::Mat::zeros(1, m_labelCount, CV_32FC1);
//        cv::Mat original = unpad(m_testImagesVector[i]);
        //qDebug() << QString::number(i);
        //qDebug() << QString::number(i) << "labeled";
//        cv::Mat colored_img = colorCoder(prediction,original);
//        cv::imshow("Colored Image",colored_img );
//        cv::waitKey();
        qDebug()<<"classfy : " << QString::number(i);
        cv::Mat label_image = classify(i);
        qDebug()<<" gettin votes : ";
        getImageLabelVotes(label_image, votes);
        std::cout<<votes<<std::endl;
        int label = getMaxLikelihoodIndex(votes);
        qDebug()<<" LABELED : "<< char('a' + label);
       // ++votes_result.at<float>(label);
        votes.release();
    }

    //qDebug()<<"RESULTING LABEL : "<< getMaxLikelihoodIndex(votes_result);

}

// checks if pixels of a letter belong to that letter
bool RandomDecisionForest::test(const cv::Mat& image, char letter, const Tree &tree)
{
    int dummy = 0 ;  // sample id dummy at the moment !s
    ImageInfo* img_Info = new ImageInfo(letter, dummy);
    std::vector<Pixel*> letterPixels;
    imageToPixels(letterPixels, image, img_Info);

    cv::Mat hist = cv::Mat::zeros(1,m_labelCount,cv::DataType<float>::type);
    for (std::vector<Pixel*>::iterator it = letterPixels.begin() ; it != letterPixels.end(); ++it)
    {
        Pixel* px = *it;
        //qDebug() << "TREE  " << tree.size();
        Node *leaf = getLeafNode(px, 0, tree);
        int index = getMaxLikelihoodIndex(leaf->m_hist);
        hist.at<float>(0,index)++;
    }

    int letterIndexPredicted = getMaxLikelihoodIndex(hist);
    int letterIndexOriginal  = letterIndex(letter);
    return(letterIndexPredicted==letterIndexOriginal);
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
        trainTree();
        RandomDecisionTree trainedTree;
        trainedTree.m_tree = m_tempTree;
        trainedTree.m_depth = m_depth;
        trainedTree.m_numberofleaves = m_numOfLeaves;
        m_forest.push_back(trainedTree);
        m_tempTree = std::vector<Node*> (m_tempTree.size());
    }
    qDebug()<< "Forest Size : " << m_forest.size();
}




// return the index of the bin with maximum number of pixels


// starting from the node with nodeID
// recursively find the leaf node of a pixel belongs to
// fix tree in recursive




// create child nodes from nodes by tuning each node
void RandomDecisionForest::constructTree( Node& root, std::vector<Pixel*>& pixels) {

    //float rootEntropy = calculateEntropyOfVector(pixels);
    int current_depth = log2(root.m_id) + 1;
    if(current_depth>m_depth)
        m_depth = current_depth;
    //no more child construction
    // min etropy removed
    if( current_depth >= MAX_DEPTH || pixels.size() <= m_minLeafPixelCount ){
        m_tempTree[root.m_id-1]->m_isLeaf = true;
        m_tempTree[root.m_id-1]->m_hist = createHistogram(pixels);
        m_numOfLeaves++;
        return;
    }

    tuneParameters(pixels, root);
    std::vector<Pixel*> left;
    std::vector<Pixel*> right;
    divide(pixels, left, right, root);


    //if(left.size()>0)
    //{
        Node *leftChildNode = new Node(2*root.m_id,false);
        leftChildNode->m_tau = generateTau();
        generateTeta(leftChildNode->m_teta1);
        generateTeta(leftChildNode->m_teta2);
        m_tempTree[leftChildNode->m_id-1] = leftChildNode;
        constructTree(*leftChildNode,left);
    //}

    //if(right.size()>0)
    //{
        Node *rightChildNode = new Node(2*root.m_id+1, false);
        rightChildNode->m_tau = generateTau();
        generateTeta(rightChildNode->m_teta1);
        generateTeta(rightChildNode->m_teta2);
        m_tempTree[rightChildNode->m_id-1] = rightChildNode;
        constructTree(*rightChildNode,right);
    //}
}

// find best teta and taw parameters for the given node
void RandomDecisionForest::tuneParameters(std::vector<Pixel*>& parentPixels, Node& parent)
{
    std::vector<Pixel*> left;
    std::vector<Pixel*> right;
    int maxTau;
    Coord maxTeta1,maxTeta2;
    float maxGain=0;
    int itr=0;

    while(itr <  m_maxIterationForDivision)
    {
        //printNode(parent);
        divide(parentPixels, left, right, parent);
        //qDebug() << "Left " << left.size() << "Right " << right.size();
        float leftChildEntr  = calculateEntropyOfVector(left);
        //qDebug() << "EntLeft" << leftChildEntr ;
        float rightChildEntr = calculateEntropyOfVector(right);
        //qDebug() << "EntRight" << rightChildEntr ;
        int sizeLeft  = left.size();
        int sizeRight = right.size();
        float totalsize = parentPixels.size() ;
        float avgEntropyChild  = (sizeLeft/totalsize) * leftChildEntr;
              avgEntropyChild += (sizeRight/totalsize)* rightChildEntr;

        float parentEntr = calculateEntropyOfVector(parentPixels);
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
        generateTeta(parent.m_teta1);
        generateTeta(parent.m_teta2);
        parent.m_tau = generateTau();
    }

    parent.m_tau = maxTau;
    parent.m_teta1 = maxTeta1;
    parent.m_teta2 = maxTeta2;
}

int RandomDecisionForest::pixelCloudSize()
{
    return m_pixelCloud.size();
}

void RandomDecisionForest::printHistogram(cv::Mat &hist)
{
    QString res = "HIST {";
    for(int i=0;i<hist.cols;i++)
        res += " " + QString::number( hist.at<float>(0,i));
    qDebug() << res << "}";
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

void RandomDecisionForest::printTree(RandomDecisionTree tree)
{
    qDebug() << "Size  : " << tree.m_tree.size() ;
    qDebug() << "Depth : " << tree.m_depth;
    qDebug() << "Leaves: " << tree.m_numberofleaves;
    int count = 0 ;
    for (auto *node : tree.m_tree)
    {
        if(node->m_isLeaf)
        {
            ++count;
            printHistogram(node->m_hist);
        }
    }
    qDebug()<< "Number of leaves : " << count ;
}
