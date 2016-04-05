#include <QFileDialog>
#include <QImage>
#include <QDialog>
#include <QObject>

#include "include/RandomDecisionForest.h"
#include "include/Reader.h"

// given the directory of the all samples
// read subsampled part of the images into pixel cloud
void RandomDecisionForest::readTrainingImageFiles(){
    //    dir = "/home/mahiratmis/Desktop/AnnotationResults"; // Got make dir generic here.
    m_dir = m_trainpath;
    std::vector<QString> fNames;
    auto *reader = new Reader();
    reader->findImages(m_dir, "", fNames);
    m_numOfLetters = fNames.size();

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
        cv::copyMakeBorder(image, image, probe_distanceY, probe_distanceY, probe_distanceX, probe_distanceX, cv::BORDER_CONSTANT);
        imagesVector.push_back(image);

        //sample image info : label : a , Id : index of in the image vector of the image.
        QString letter = fileName.split("_")[0];
        // store label of the image in m_labels vector
        m_labels.push_back(letter[0].toLatin1());
    }
    qDebug()<<"No of IMAGES : " << imagesVector.size() << " NO of Fnames" <<m_numOfLetters;
    fNames.clear();
}

void RandomDecisionForest::subSample(){

    int pixelCloudSize = PIXELS_PER_IMAGE*m_numOfLetters;
    int sampleId=0;

    pixelCloud.clear();

    for (auto it = std::begin(imagesVector) ; it != std::end(imagesVector); ++it)
    {
        auto image = *it;
        auto label = m_labels[sampleId];
        ImageInfo *img_inf = new ImageInfo(label,sampleId++);

        // draw perImagePixel pixels from image
        // bootstrap, subsample
        for(int k=0;k<PIXELS_PER_IMAGE; ++k)
        {
            int i = (rand() % image.rows-2*probe_distanceY) + probe_distanceY;
            int j = (rand() % image.cols-2*probe_distanceX) + probe_distanceX;
            auto intensity = image.at<uchar>(i,j);
            auto *px = new Pixel(Coord(i,j),intensity,img_inf);
            pixelCloud.push_back(px);
        }
    }
}

//given the image path, fills the vector with in the pixels of the image
void RandomDecisionForest::imageToPixels(std::vector<Pixel*> &res, QString& filePath,ImageInfo* img_inf ){
    cv::Mat image = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

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
    for(auto pPixel : pixelCloud)
        printPixel(pPixel);
}

void RandomDecisionForest::printPixel(Pixel* px){
    qDebug() << "Pixel{ Coor("    << px->position.x     << ","     << px->position.y
             << ") Label("        << px->imgInfo->label << ") Id(" << px->imgInfo->sampleId
             << ") = "            << px->intensity << "}";
}

//random teta value
void RandomDecisionForest::generateTeta(Coord& crd)
{
    // random number between -probe_distance, probe_distance
    crd.x = (rand() % (2*probe_distanceX)) - probe_distanceX;
    crd.y = (rand() % (2*probe_distanceY)) - probe_distanceY;
}

//random taw value
int RandomDecisionForest::generateTaw()
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
    int new_teta1X = node.teta1.x + p->position.x;
    int new_teta1Y = node.teta1.y + p->position.y;

    bool bIsOutOfRangeX = (new_teta1X > img.cols || new_teta1X < 0);
    bool bIsOutOfRangeY = (new_teta1Y > img.rows || new_teta1Y < 0);

    unsigned char intensity1=0;
    if(!(bIsOutOfRangeX || bIsOutOfRangeY)){
        intensity1 = img.at<uchar>(new_teta1X,new_teta1Y);
    }

    int new_teta2X = node.teta2.x + p->position.x ;
    int new_teta2Y = node.teta2.y + p->position.y ;

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

    return ((intensity1- intensity2) <= node.taw);
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
    root.taw = generateTaw();
    generateTeta(root.teta1);
    generateTeta(root.teta2);
    m_tempTree[root.id-1] = root;
    m_depth=1;
    constructTree(root,pixelCloud);
    qDebug() << "TREE CONSTRUCTED: LEAVES:" << numOfLeaves << " LETTER SAMPLES: " << m_numOfLetters;

//    printTree();


}

// checks if pixels of a letter belong to that letter
bool RandomDecisionForest::test(std::vector<Pixel*>& letterPixels, char letter){
    cv::Mat hist = cv::Mat::zeros(1,NUM_LABELS,cv::DataType<float>::type);
    for (std::vector<Pixel*>::iterator it = letterPixels.begin() ; it != letterPixels.end(); ++it){
        Pixel* px = *it;
        //qDebug() << "TREE  " << tree.size();
        Node leaf = getLeafNode(px, 0);
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
    this->testpath = path;
}

void RandomDecisionForest::trainForest()
{
    for (int i = 0; i < m_no_of_trees; ++i) {
        trainTree();
        RandomDecisionTree trainedTree;
        trainedTree.m_tree = m_tempTree;
        trainedTree.m_depth = m_depth;
        trainedTree.m_numberofleaves = numOfLeaves;
        m_forest.push_back(trainedTree);
        m_tempTree.clear();
    }
    qDebug()<< "Forest Size : " << m_forest.size();
}


// return the index of the bin with maximum number of pixels


// starting from the node with nodeID
// recursively find the leaf node the pixel belongs to
Node RandomDecisionForest::getLeafNode(Pixel*px, int nodeId){

    Node root = m_tempTree[nodeId];
    if(root.isLeaf) {
        // qDebug()<<"LEAF REACHED :"<<root.id;
        return root;
    }

    cv::Mat img = imagesVector[px->imgInfo->sampleId];
    int childId = root.id *2 ;
    //qDebug()<<"LEAF SEARCH :"<<root.id << " is leaf : " << root.isLeaf;
    if(!isLeft(px,root,img))
        childId++;
    return getLeafNode(px,childId-1);
}



// create child nodes from nodes by tuning each node
void RandomDecisionForest::constructTree( Node& root, std::vector<Pixel*>& pixels) {

    float rootEntropy = calculateEntropyOfVector(pixels);
    int current_depth = log2(root.id) + 1;
    //qDebug()<< " curr depth : " << current_depth << "root id :" << root.id;
    if(current_depth>m_depth)
        m_depth = current_depth;
    //qDebug() << "Root ID : "<< root.id;

    //no more child construction

    if(MIN_ENTROPY > rootEntropy || current_depth >= MAX_DEPTH || pixels.size() <= MIN_LEAF_PIXELS ){
        //qDebug() << "Leaf Reached";
        m_tempTree[root.id-1].isLeaf = true;
        m_tempTree[root.id-1].hist = createHistogram(pixels);
//        qDebug() << "Leaf Id:" << root.id;
        //printHistogram(root.hist);
        numOfLeaves++;
        return;
    }

    //qDebug() << "Tuning Parameters";
    tuneParameters(pixels, root);
    //qDebug() << "Tuned Parameters";

    std::vector<Pixel*> left;
    std::vector<Pixel*> right;

    //qDebug() << "Dividing Pixels";
    divide(pixels,left,right,root);
    //qDebug() << "Left " << left.size() << "Right " << right.size();
    //qDebug() << "Divided Pixels";

    // qDebug() << "Tree Max İndex: " << tree.size();
    // NEEDS FIX
    Node leftChildNode;
    leftChildNode.id = 2*root.id;
    leftChildNode.isLeaf = false;
    leftChildNode.taw = generateTaw();
    generateTeta(leftChildNode.teta1);
    generateTeta(leftChildNode.teta2);
    m_tempTree[leftChildNode.id-1] = leftChildNode;
    //    qDebug() << "Left Child :" <<leftChildNode.isLeaf  <<" ||  m_Tree  :"  << m_tree[leftChildNode.id-1].isLeaf;
    constructTree(leftChildNode,left);

    //    qDebug() <<"Is" << leftChildNode.id << " is Leaf :" << leftChildNode.isLeaf;

    Node rightChildNode;
    rightChildNode.id = 2*root.id+1;
    rightChildNode.isLeaf=false;
    rightChildNode.taw = generateTaw();
    generateTeta(rightChildNode.teta1);
    generateTeta(rightChildNode.teta2);
    m_tempTree[rightChildNode.id-1] = rightChildNode;
    //  qDebug() << "Right Child " << QString::number(tree[rightChildNode.id-1].id) << "Created LEAF :"<< tree[rightChildNode.id-1].isLeaf;
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
            maxTaw   = parent.taw;
            maxGain = infoGain;
            itr = 0 ;
        }
        else
            ++itr;

        left.clear();
        right.clear();
        generateTeta(parent.teta1);
        generateTeta(parent.teta2);
        parent.taw = generateTaw();
    }

    parent.taw = maxTaw;
    parent.teta1 = maxTeta1;
    parent.teta2 = maxTeta2;
}

int RandomDecisionForest::pixelCloudSize()
{
    return pixelCloud.size();
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
             << "Taw: " << node.taw
             << "Q1{" << node.teta1.x << "," << node.teta1.y
             << "Q2{" << node.teta2.x << "," << node.teta2.y;
    printHistogram(node.hist);
    qDebug() << "}";
}

void RandomDecisionForest::printTree(RandomDecisionTree tree)
{
    qDebug() << "Size  : " << tree.m_tree.size() ;
    qDebug() << "Depth : " << tree.m_depth;
    qDebug() << "Leaves: " << tree.m_numberofleaves;
    int count = 0 ;
    for (auto it = std::begin(tree.m_tree) ; it != std::end(tree.m_tree); ++it)
    {
        Node node = *it;

        if(node.isLeaf)
        {
            //                      qDebug() << "Leaf id :" << node.id <<"is leaf: " << node.isLeaf;
            ++count;
        }


    }
    qDebug()<<"Number of leaves kept : " << count ;

}
