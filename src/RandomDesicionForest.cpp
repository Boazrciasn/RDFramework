#include "include/RandomDesicionForest.h"
#include "include/Reader.h"
#include <QFileDialog>
#include <QImage>

// given the directory of the all samples
// read subsampled part of the images into pixel cloud
void RandomDesicionForest::readTrainingImageFiles(){
    /* initialize random seed: */
    srand (time(NULL));

    min_InfoGain =1;
    max_InfoGain =-1;

   //QString dir = QFileDialog::getExistingDirectory(this,tr("Open Image Direrctory"), QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
   dir = "/home/mahiratmis/Desktop/AnnotationResults"; // Got make dir generic here.
   vector<QString>fNames;
   Reader * reader = new Reader();
   reader->findImages(dir, "", fNames);
   numOfLetters = fNames.size();

   int perImagePixel = PIXEL_CLOUD_SIZE/numOfLetters;

   for (vector<QString>::iterator it = fNames.begin() ; it != fNames.end(); ++it)
   {
       QString filePath = *it;
       QString fileName = Util::fileNameWithoutPath(filePath) ;
       //each directory contains lastsession.txt
       if(!fileName.contains("_")) continue;

       //sample filenames a_0, a_11
       QString letter = fileName.split("_")[0];
       QString sampleId = fileName.split("_")[1];
       int id = sampleId.toInt();
       ImageInfo* img_inf = new ImageInfo(letter[0].toLatin1(),id);

       filePath += "/" + fileName + ".jpg";
       Mat image = imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

       // draw perImagePixel pixels from image
       // bootstrap, subsample
       for(int k=0;k<perImagePixel;k++){
            int i = rand() % image.rows;
            int j = rand() % image.cols;
            unsigned char intensity = image.at<uchar>(i,j);
            Pixel* px = new Pixel(Coord(i,j),intensity,img_inf);
            pixelCloud.push_back(px);
       }
       image.release();
   }
   fNames.clear();
}

//given the image path, fills in the pixel vector
void RandomDesicionForest::imageToPixels(vector<Pixel*>& res, QString& filePath,ImageInfo* img_inf ){
   Mat image = imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
   for(int i =0; i<image.rows;i++){
       for(int j =0; j<image.cols;j++){
            unsigned char intensity = image.at<uchar>(i,j);
            Pixel* px = new Pixel(Coord(i,j),intensity,img_inf);
            res.push_back(px);
       }
   }
}

void RandomDesicionForest::printPixelCloud(){
   for (vector<Pixel*>::iterator it = pixelCloud.begin() ; it != pixelCloud.end(); ++it){
       printPixel(*it);
   }
}

void RandomDesicionForest::printPixel(Pixel* px){
    qDebug() << "Pixel{ Coor("    << px->position.x     << ","     << px->position.y
             << ") Label("        << px->imgInfo->label << ") Id(" << px->imgInfo->sampleId
             << ") = "            << px->intensity << "}";
}

//random teta value
void RandomDesicionForest::generateTeta(Coord& crd)
{
    // random number between -probe_distance, probe_distance
    crd.x = (rand() % (2*probe_distanceX)) - probe_distanceX;
    crd.y = (rand() % (2*probe_distanceY)) - probe_distanceY;
}

//random taw value
int RandomDesicionForest::generateTaw()
{
    // random number between -127, +127
    return (rand() % 256) - 127;
}

float RandomDesicionForest::calculateEntropy(Mat& hist)
{
    float entr = 0;
    float totalSize=0;
    for(int i=0;i<hist.cols;i++){
       totalSize += hist.at<float>(0, i);
    }

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

float RandomDesicionForest::calculateEntropyOfVector(vector<Pixel*>& pixels)
{
    Mat hist = createHistogram(pixels);
    return calculateEntropy(hist);
}

// creates histogram out of a pixel vector
Mat RandomDesicionForest::createHistogram(vector<Pixel*>& pixels){
    Mat hist = Mat::zeros(1,NUM_LABELS,DataType<float>::type);
    for (vector<Pixel*>::iterator it = pixels.begin() ; it != pixels.end(); ++it)
    {
        Pixel* px = *it;
        int index = letterIndex(px->imgInfo->label);
        hist.at<float>(0,index)++;
    }
    return hist;
}

// given a letter returns its index on label histogram
int RandomDesicionForest::letterIndex(char letter){
    int asciiVal = (int)letter;
    int asciiA = (int)'a';
    return asciiVal-asciiA;
}

// checks if a pixel will travel to the left of a given node
bool RandomDesicionForest::isLeft(Pixel* p, Node&node, Mat& img)
{
    int new_teta1X = node.teta1.x + p->position.x ;
    int new_teta1Y = node.teta1.y + p->position.y ;

    bool isOutOfRangeX = (new_teta1X > img.cols || new_teta1X < 0);
    bool isOutOfRangeY = (new_teta1Y > img.rows || new_teta1Y < 0);

    unsigned char intensity1=0;
    if(!(isOutOfRangeX || isOutOfRangeY)){
        intensity1 = img.at<uchar>(new_teta1X,new_teta1Y);
    }

    int new_teta2X = node.teta2.x + p->position.x ;
    int new_teta2Y = node.teta2.y + p->position.y ;

    isOutOfRangeX = (new_teta2X > img.cols || new_teta2X < 0);
    isOutOfRangeY = (new_teta2Y > img.rows || new_teta2Y < 0);

    unsigned char intensity2=0;
    if(!(isOutOfRangeX || isOutOfRangeY)){
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
void RandomDesicionForest::divide(vector<Pixel*>& parentPixels,
                                  vector<Pixel*>& left,
                                  vector<Pixel*>& right, Node& parent){

    for (vector<Pixel*>::iterator it = parentPixels.begin() ; it != parentPixels.end(); ++it)
    {
        Pixel* px = *it;
        Mat img = getPixelImage(px);
        if(isLeft(px, parent, img)){
            left.push_back(px);
        }else{
            right.push_back(px);
        }
    }
}

// returns the image the pixel belongs to
Mat RandomDesicionForest::getPixelImage(Pixel* px){
    QString path = dir + "/"+ px->imgInfo->label + "/" + px->imgInfo->label + "_"
                       + QString::number(px->imgInfo->sampleId)  + ".jpg";
    return imread(path.toStdString());
}

// create the tree
void RandomDesicionForest::train()
{
    Node root;
    root.id =1;
    root.isLeaf=false;
    root.taw = generateTaw();
    generateTeta(root.teta1);
    generateTeta(root.teta2);
    vector<Node>treee(pow(2,NUM_LABELS)-1);
    treee[root.id-1] = root;
    depth=1;
    constructTree(treee,root,pixelCloud);
    tree = treee;
    qDebug() << "TREE CONSTRUCTED: LEAVES:" << numOfLeaves << " LETTER SAMPLES: " << numOfLetters;
}

// checks if pixels of a letter belong to that letter
bool RandomDesicionForest::test(vector<Pixel*>& letterPixels, char letter){
    Mat hist = Mat::zeros(1,NUM_LABELS,DataType<float>::type);
    for (vector<Pixel*>::iterator it = letterPixels.begin() ; it != letterPixels.end(); ++it){
        Pixel* px = *it;
        //qDebug() << "TREE  " << tree.size();
        Node leaf = getLeafNode(px, 0);
        int index = getLabelIndex(leaf.hist);
        hist.at<float>(0,index)++;
    }

    int letterIndexPredicted = getLabelIndex(hist);
    int letterIndexOriginal  = letterIndex(letter);
    return(letterIndexPredicted==letterIndexOriginal);
}

// return the index of the bin with maximum number of pixels
int RandomDesicionForest::getLabelIndex(Mat& hist){
    int max_val=-1;
    int max_index=0;
    for(int i=0;i<hist.cols;i++){
        if(hist.at<float>(0,i) > max_val){
            max_val = hist.at<float>(0,i);
            max_index= i;
        }
    }
    return max_index;
}

// starting from the node with nodeID
// recursively find the leaf node the pixel belongs to
Node RandomDesicionForest::getLeafNode(Pixel*px, int nodeId){

    Node root = tree[nodeId];
    if(root.isLeaf) {
        return root;
    }

    Mat img = getPixelImage(px);
    int childId = root.id *2 ;
    if(!isLeft(px,root,img))
      childId++;
    return getLeafNode(px,childId);
}

// create child nodes from nodes by tuning each node
void RandomDesicionForest::constructTree(vector<Node>& tree, Node& root, vector<Pixel*>& pixels) {

    float rootEntropy = calculateEntropyOfVector(pixels);
    int current_depth = log2(root.id);
    //no more child construction
    if(MIN_ENTROPY > rootEntropy || current_depth >= MAX_DEPTH){
        //qDebug() << "Leaf Reached";
        root.isLeaf = true;
        root.hist = createHistogram(pixels);
        //qDebug() << "Leaf at " << depth << " CurrDepth:" << current_depth;
        //printHistogram(root.hist);
        numOfLeaves++;
        return;
    }

    //qDebug() << "Tuning Parameters";
    tuneParameters(pixels,root);
    //qDebug() << "Tuned Parameters";

    vector<Pixel*> left;
    vector<Pixel*> right;

    //qDebug() << "Dividing Pixels";
    divide(pixels,left,right,root);
    //qDebug() << "Left " << left.size() << "Right " << right.size();
    //qDebug() << "Divided Pixels";

    //qDebug() << "Tree Max İndex: " << tree.size();
    Node leftChildNode;
    leftChildNode.id = 2*root.id;
    //qDebug() << "Left Child " << QString::number(leftChildNode.id) << "Created";
    tree[leftChildNode.id-1] = leftChildNode;
    leftChildNode.isLeaf=false;
    leftChildNode.taw = generateTaw();
    generateTeta(leftChildNode.teta1);
    generateTeta(leftChildNode.teta2);
    constructTree(tree,leftChildNode,left);

    Node rightChildNode;
    rightChildNode.id = 2*root.id+1;
    //qDebug() << "Right Child " << QString::number(rightChildNode.id) << "Created";
    tree[rightChildNode.id-1] = rightChildNode;
    rightChildNode.isLeaf=false;
    rightChildNode.taw = generateTaw();
    generateTeta(rightChildNode.teta1);
    generateTeta(rightChildNode.teta2);
    constructTree(tree,rightChildNode,right);
}

// find best teta and taw parameters for the given node
void RandomDesicionForest::tuneParameters(vector<Pixel*>& parentPixels, Node& parent)
{
    vector<Pixel*> left;
    vector<Pixel*> right;
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
        float totalsize = sizeLeft + sizeRight ;
        float avgEntropyChild  = (sizeLeft/totalsize) * leftChildEntr;
              avgEntropyChild += (sizeRight/totalsize)* rightChildEntr;

        float parentEntr = calculateEntropyOfVector(parentPixels);
        float infoGain = parentEntr - avgEntropyChild;
        //qDebug() << "InfoGain" << infoGain ;

        // Non-improving epoch here !
        if(infoGain > max_InfoGain)
            max_InfoGain = infoGain;
        else if(infoGain < min_InfoGain)
            min_InfoGain = infoGain;

        if(infoGain > maxGain){
            maxTeta1 = parent.teta1;
            maxTeta2 = parent.teta2;
            maxTaw   = parent.taw;
        }

       left.clear();
       right.clear();
       generateTeta(parent.teta1);
       generateTeta(parent.teta2);
       parent.taw = generateTaw();
       itr++;
    }

    parent.taw = maxTaw;
    parent.teta1 = maxTeta1;
    parent.teta2 = maxTeta2;
}

int RandomDesicionForest::pixelCloudSize()
{
    return pixelCloud.size();
}

void RandomDesicionForest::printHistogram(Mat &hist)
{
    QString res = "HIST {";
    for(int i=0;i<hist.cols;i++)
        res += " " + QString::number( hist.at<float>(0,i));
    qDebug() << res << "}";
}

void RandomDesicionForest::printNode(Node& node){
    QString res = "NODE {";
    qDebug() << "Id:"   << node.id
             << "Taw: " << node.taw
             << "Q1{" << node.teta1.x << "," << node.teta1.y
             << "Q2{" << node.teta2.x << "," << node.teta2.y;
    printHistogram(node.hist);
    qDebug() << "}";
}
