#include "precompiled.h"

#include "include/RandomDecisionForest.h"
#include "include/Reader.h"
#include "include/Util.h"


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
        cv::copyMakeBorder(image, image, m_params.probDistY, m_params.probDistY, m_params.probDistX, m_params.probDistX, cv::BORDER_CONSTANT);
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
        cv::copyMakeBorder(image, image, m_params.probDistY, m_params.probDistY, m_params.probDistX, m_params.probDistX, cv::BORDER_CONSTANT);
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
            color.at<double>(i,j) = (label/m_params.labelCount)*255;
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
    for (int i = 0; i < m_no_of_trees; ++i)
    {
        qDebug()<< "Tree number " << QString::number(i+1) << "is being trained" ;
        auto *trainedRDT = new RandomDecisionTree(this);
        trainedRDT->setMaxDepth(m_params.maxDepth);
        qDebug()<< "Train..." ;
        trainedRDT->train();
        m_forest.push_back(trainedRDT);
    }
    qDebug()<< "Forest Size : " << m_forest.size();
}
