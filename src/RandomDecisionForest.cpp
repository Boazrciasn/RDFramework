#include <QFileDialog>
#include <QImage>
#include <QDialog>
#include <QObject>

#include "include/RandomDecisionForest.h"
#include "include/Reader.h"


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

    m_dir = m_testpath;
    std::vector<QString> fNames;
    auto *reader = new Reader();
    reader->findImages(m_dir, "", fNames);
    for (auto filePath : fNames)
    {
        QString fileName = Util::fileNameWithoutPath(filePath);
        qDebug() << fileName ;
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
        for(int k=0;k<PIXELS_PER_IMAGE; ++k)
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
void RandomDecisionForest::imageToPixels(std::vector<Pixel*> &res,const cv::Mat &image ,ImageInfo* img_inf ){

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
    qDebug() << "Pixel{ Coor("    << px->position.m_x     << ","     << px->position.m_y
             << ") Label("        << px->imgInfo->label << ") Id(" << px->imgInfo->sampleId
             << ") = "            << px->intensity << "}";
}

//random teta value
void RandomDecisionForest::generateTeta(Coord& crd)
{
    // random number between -probe_distance, probe_distance
    crd.m_x = (rand() % (2*m_probe_distanceX)) - m_probe_distanceX;
    crd.m_y = (rand() % (2*m_probe_distanceY)) - m_probe_distanceY;
}

//random tau value
int RandomDecisionForest::generateTau()
{
    // random number between -127, +128
    return (rand() % 256) - 127;
}

float RandomDecisionForest::calculateEntropy(cv::Mat& hist)
{
    float entr = 0;
    float totalSize=0;
    int nCols = hist.cols;

    for(int i=0; i<nCols; ++i)
        totalSize += hist.at<float>(0, i);

    for(int i=0;i<hist.cols;i++){
        float nPixelsAt = hist.at<float>(0, i);
        if(nPixelsAt>0){
            float probability = nPixelsAt/totalSize;
            entr += probability*(log2(1.0f/probability));
        }
    }
    //cout << "ENTROPY : " << entr;
    return entr;
}

float RandomDecisionForest::calculateEntropyOfVector(std::vector<Pixel*>& pixels)
{
    cv::Mat hist = createHistogram(pixels);
    return calculateEntropy(hist);
}

// creates histogram out of a pixel vector : need(?) fix after image info re-arrange.
cv::Mat RandomDecisionForest::createHistogram(std::vector<Pixel*>& pixels){
    cv::Mat hist = cv::Mat::zeros(1,NUM_LABELS,cv::DataType<float>::type);
    for (std::vector<Pixel*>::iterator it = pixels.begin() ; it != pixels.end(); ++it)
    {
        Pixel* px = *it;
        int index = letterIndex(px->imgInfo->label);
        hist.at<float>(0,index)++;
    }
    return hist;
}

// given a letter returns its index on label histogram


// checks if a pixel will travel to the left of a given node
bool RandomDecisionForest::isLeft(Pixel* p, Node&node, cv::Mat& img)
{
    int new_teta1X = node.teta1.m_x + p->position.m_x;
    int new_teta1Y = node.teta1.m_y + p->position.m_y;

    bool bIsOutOfRangeX = (new_teta1X > img.cols || new_teta1X < 0);
    bool bIsOutOfRangeY = (new_teta1Y > img.rows || new_teta1Y < 0);

    unsigned char intensity1=0;
    if(!(bIsOutOfRangeX || bIsOutOfRangeY)){
        intensity1 = img.at<uchar>(new_teta1X,new_teta1Y);
    }

    int new_teta2X = node.teta2.m_x + p->position.m_x ;
    int new_teta2Y = node.teta2.m_y + p->position.m_y ;

    bIsOutOfRangeX = (new_teta2X > img.cols || new_teta2X < 0);
    bIsOutOfRangeY = (new_teta2Y > img.rows || new_teta2Y < 0);

    unsigned char intensity2=0;
    if(!(bIsOutOfRangeX || bIsOutOfRangeY)){
        intensity2 = img.at<uchar>(new_teta2X,new_teta2Y);
    }

    //    qDebug() << "Img ("<< img.cols << "," << img.rows << ")";
    //    qDebug() << "Q1 ("<< new_teta1X << "," << new_teta1Y << ")";
    //    qDebug() << "Q2 ("<< new_teta2X << "," << new_teta2Y << ")";
    //    qDebug() << "Pixels ("<< intensity1 << "," << intensity2 << ")";
    //    qDebug() << "Taw ("<< node.taw << ")";

    return ((intensity1- intensity2) <= node.tau);
}

// divide pixel vector into 2 parts according to the parameters of parent node


// returns the image the pixel belongs to
cv::Mat RandomDecisionForest::getPixelImage(Pixel* px){
    QString path = m_dir + "/"+ px->imgInfo->label + "/" + px->imgInfo->label + "_"
            + QString::number(px->imgInfo->sampleId)  + ".jpg";
    //qDebug()<<"IMAGE :"<< path;
    return cv::imread(path.toStdString());
}

// create the tree
void RandomDecisionForest::trainTree()
{
    subSample();
    Node root;
    root.id = 1;
    root.isLeaf = false;
    root.tau = generateTau();
    generateTeta(root.teta1);
    generateTeta(root.teta2);
    qDebug()<<"Temp Tree Size trainTree: "<< m_tempTree.size();
    m_tempTree[root.id-1] = root;
    m_depth=1;
    m_numOfLeaves = 0 ;
    constructTree(root,m_pixelCloud);
    qDebug() << "TREE CONSTRUCTED: LEAVES:" << m_numOfLeaves << " LETTER SAMPLES: " << m_numOfLetters;
    qDebug() << "Pixel Sizes Are" << (pixelSizesConsistent() ? "Consistent" : "Not Consistent") ;
//  printTree();
}

bool RandomDecisionForest::pixelSizesConsistent()
{
    auto nPixelsOnLeaves=0u;
    for(Node node : m_tempTree)
    {
        if(node.isLeaf)
            nPixelsOnLeaves += getTotalNumberOfPixels(node.hist);
    }

    return m_pixelCloud.size() == nPixelsOnLeaves;
}

//padded image index must be provided
cv::Mat RandomDecisionForest::classify(int index)
{
    cv::Mat test_image = m_testImagesVector[index];
    int n_rows=test_image.rows;
    int n_cols=test_image.cols;
    cv::Mat res_image = cv::Mat(n_rows, n_cols,test_image.type());
    ImageInfo* img_Info = new ImageInfo(' ', index);
    for(int r=m_probe_distanceY;r<n_rows-m_probe_distanceY; ++r)
    {
        for(int c=m_probe_distanceX;c<n_cols-m_probe_distanceX; ++c)
        {
            auto intensity = test_image.at<uchar>(r,c);
            auto *px = new Pixel(Coord(r,c),intensity,img_Info);
            cv::Mat probHist = cv::Mat::zeros(1,NUM_LABELS,cv::DataType<float>::type);

            for(auto tree : m_forest)
            {
                Node leaf = getLeafNode(px, 0, tree.m_tree);
                probHist += leaf.hist;
            }
            auto label = getMaxLikelihoodIndex(probHist);
            res_image.at<uchar>(r,c) = label;
        }
    }
    return res_image;
}

void RandomDecisionForest::test()
{
    int size = m_testImagesVector.size();
    for(auto i=0; i<size; i++)
    {
        cv::Mat prediction = classify(i);
    }
}

// checks if pixels of a letter belong to that letter
bool RandomDecisionForest::test(const cv::Mat& image, char letter, const Tree &tree)
{
    int dummy = 0 ;  // sample id dummy at the moment !s
    ImageInfo* img_Info = new ImageInfo(letter, dummy);
    std::vector<Pixel*> letterPixels;
    imageToPixels(letterPixels, image, img_Info);

    cv::Mat hist = cv::Mat::zeros(1,NUM_LABELS,cv::DataType<float>::type);
    for (std::vector<Pixel*>::iterator it = letterPixels.begin() ; it != letterPixels.end(); ++it)
    {
        Pixel* px = *it;
        //qDebug() << "TREE  " << tree.size();
        Node leaf = getLeafNode(px, 0, tree);
        int index = getMaxLikelihoodIndex(leaf.hist);
        hist.at<float>(0,index)++;
    }

    int letterIndexPredicted = getMaxLikelihoodIndex(hist);
    int letterIndexOriginal  = letterIndex(letter);
    return(letterIndexPredicted==letterIndexOriginal);
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
    for (int i = 0; i < m_no_of_trees; ++i) {
        trainTree();
        RandomDecisionTree trainedTree;
        trainedTree.m_tree = m_tempTree;
        trainedTree.m_depth = m_depth;
        trainedTree.m_numberofleaves = m_numOfLeaves;
        m_forest.push_back(trainedTree);
        m_tempTree = std::vector<Node> (m_tempTree.size());

    }
    qDebug()<< "Forest Size : " << m_forest.size();
}


// return the index of the bin with maximum number of pixels


// starting from the node with nodeID
// recursively find the leaf node of a pixel belongs to
// fix tree in recursive
Node RandomDecisionForest::getLeafNode(Pixel*px, int nodeId, const Tree &tree)
{

    Node root = tree[nodeId];
    if(root.isLeaf)
    {
        // qDebug()<<"LEAF REACHED :"<<root.id;
        return root;
    }

    cv::Mat img = m_testImagesVector[px->imgInfo->sampleId];
    int childId = root.id *2 ;
    //qDebug()<<"LEAF SEARCH :"<<root.id << " is leaf : " << root.isLeaf;
    if(!isLeft(px,root,img))
        childId++;
    return getLeafNode(px,childId-1, tree);
}



// create child nodes from nodes by tuning each node
void RandomDecisionForest::constructTree( Node& root, std::vector<Pixel*>& pixels) {

    float rootEntropy = calculateEntropyOfVector(pixels);
    int current_depth = log2(root.id) + 1;
    if(current_depth>m_depth)
        m_depth = current_depth;
    //no more child construction
    if(MIN_ENTROPY > rootEntropy || current_depth >= MAX_DEPTH || pixels.size() <= MIN_LEAF_PIXELS ){
        m_tempTree[root.id-1].isLeaf = true;
        m_tempTree[root.id-1].hist = createHistogram(pixels);
        m_numOfLeaves++;
        return;
    }

    tuneParameters(pixels, root);
    std::vector<Pixel*> left;
    std::vector<Pixel*> right;
    divide(pixels,left,right,root);

    Node leftChildNode;
    leftChildNode.id = 2*root.id;
    leftChildNode.isLeaf = false;
    leftChildNode.tau = generateTau();
    generateTeta(leftChildNode.teta1);
    generateTeta(leftChildNode.teta2);
    m_tempTree[leftChildNode.id-1] = leftChildNode;
    constructTree(leftChildNode,left);

    Node rightChildNode;
    rightChildNode.id = 2*root.id+1;
    rightChildNode.isLeaf=false;
    rightChildNode.tau = generateTau();
    generateTeta(rightChildNode.teta1);
    generateTeta(rightChildNode.teta2);
    m_tempTree[rightChildNode.id-1] = rightChildNode;
    constructTree(rightChildNode,right);

}

// find best teta and taw parameters for the given node
void RandomDecisionForest::tuneParameters(std::vector<Pixel*>& parentPixels, Node& parent)
{
    std::vector<Pixel*> left;
    std::vector<Pixel*> right;
    int maxTaw;
    Coord maxTeta1,maxTeta2;
    float maxGain=0;
    int itr=0;


    while(itr <  MAX_NUMITERATION_FOR_DIVISION)
    {
        //printNode(parent);
        divide(parentPixels,left,right,parent);
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

        // Non-improving epoch here !
        if(infoGain > max_InfoGain)
            max_InfoGain = infoGain;
        else if(infoGain < min_InfoGain)
            min_InfoGain = infoGain;

        // Non-improving epoch :
        if(infoGain > maxGain)
        {
            maxTeta1 = parent.teta1;
            maxTeta2 = parent.teta2;
            maxTaw   = parent.tau;
            maxGain = infoGain;
            itr = 0 ;
        }
        else
            ++itr;

        left.clear();
        right.clear();
        generateTeta(parent.teta1);
        generateTeta(parent.teta2);
        parent.tau = generateTau();
    }

    parent.tau = maxTaw;
    parent.teta1 = maxTeta1;
    parent.teta2 = maxTeta2;
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
    qDebug() << "Id:"   << node.id
             << "Taw: " << node.tau
             << "Q1{" << node.teta1.m_x << "," << node.teta1.m_y
             << "Q2{" << node.teta2.m_x << "," << node.teta2.m_y;
    printHistogram(node.hist);
    qDebug() << "}";
}

void RandomDecisionForest::printTree(RandomDecisionTree tree)
{
    qDebug() << "Size  : " << tree.m_tree.size() ;
    qDebug() << "Depth : " << tree.m_depth;
    qDebug() << "Leaves: " << tree.m_numberofleaves;
    int count = 0 ;
    for (auto node : tree.m_tree)
    {
        if(node.isLeaf)
        {
            ++count;
            printHistogram(node.hist);
        }
    }
    qDebug()<< "Number of leaves : " << count ;

}
