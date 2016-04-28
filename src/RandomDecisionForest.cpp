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
    m_dir = m_params.trainDir;
    std::vector<QString> fNames;
    auto *reader = new Reader();
    reader->findImages(m_dir, "", fNames, m_DS.m_trainlabels);
    m_numOfLetters = fNames.size();
    qDebug()<<"NO OF LETTERS : "<< m_numOfLetters;

    for (auto filePath : fNames)
    {
        cv::Mat image = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
        //pad image and save to vector
        cv::copyMakeBorder(image, image, m_params.probDistY, m_params.probDistY, m_params.probDistX, m_params.probDistX, cv::BORDER_CONSTANT);
        m_DS.m_trainImagesVector.push_back(image);
    }
    qDebug()<<"No of IMAGES : " << m_DS.m_trainImagesVector.size() << " NO of Fnames" <<m_numOfLetters<< "Images Vector Size : "<< m_DS.m_trainImagesVector.size();
    fNames.clear();
}

void RandomDecisionForest::readTestImageFiles()
{
    m_DS.m_testImagesVector.clear();
    m_dir = m_params.testDir;
    std::vector<QString> fNames;
    auto *reader = new Reader();
    reader->findImages(m_dir, "", fNames, m_DS.m_testlabels);

    for (auto filePath : fNames)
    {
        //TODO: reads file with only
        cv::Mat image = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
        //pad image and save to vector
        cv::copyMakeBorder(image, image, m_params.probDistY, m_params.probDistY, m_params.probDistX, m_params.probDistX, cv::BORDER_CONSTANT);
        m_DS.m_testImagesVector.push_back(image);
    }
    qDebug()<<"No of test IMAGES : " << m_DS.m_testImagesVector.size();
    fNames.clear();

}

//FOR TEST PURPOSES ONLY : given the image path, fills the vector with in the pixels of the image, img_Info : label of  test image & id of the image inside vector(optional)
void RandomDecisionForest::imageToPixels(std::vector<pixel_ptr> &res,const cv::Mat &image ,imageinfo_ptr img_inf )
{
    int nRows = image.rows;
    int nCols = image.cols;
    for(int i =0; i<nRows; ++i)
    {
        for(int j =0; j<nCols; ++j)
        {
            auto intensity = image.at<uchar>(i,j);
            pixel_ptr px(new Pixel(Coord(i,j),intensity,img_inf));
            res.push_back(px);
        }
    }
}

// given a letter returns its index on label histogram
// checks if a pixel will travel to the left of a given node
// divide pixel vector into 2 parts according to the parameters of parent node
// returns the image the pixel belongs to
cv::Mat RandomDecisionForest::getPixelImage(pixel_ptr px)
{
    QString path = m_dir + "/"+ px->imgInfo->label + "/" + px->imgInfo->label + "_"
            + QString::number(px->imgInfo->sampleId)  + ".jpg";
    //qDebug()<<"IMAGE :"<< path;
    return cv::imread(path.toStdString());
}

cv::Mat RandomDecisionForest::colorCoder(const cv::Mat &labelImage, const cv::Mat &InputImage)
{
    int nRows = InputImage.rows;
    int nCols = InputImage.cols;
    int labelCount = m_params.labelCount;

    cv::Mat color(nRows, nCols, CV_8UC1);

    //create color code Mat accoridng to of each pixel.
    for(int i =0; i<nRows; ++i)
    {
        for(int j =0; j<nCols; ++j)
        {
            auto label = labelImage.at<double>(i,j);
            color.at<double>(i,j) = (label/labelCount)*255;
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


void RandomDecisionForest::trainForest()
{
    for (int i = 0; i < m_params.nTrees; ++i)
    {
        qDebug()<< "Tree number " << QString::number(i+1) << "is being trained" ;
        auto *trainedRDT = new RandomDecisionTree(this);
        trainedRDT->setProbeDistanceX(m_params.probDistX);
        trainedRDT->setProbeDistanceY(m_params.probDistY);
        trainedRDT->setMaxDepth(m_params.maxDepth);
        trainedRDT->setMinimumLeafPixelCount(m_params.minLeafPixels);
        qDebug()<< "Train..." ;
        trainedRDT->train();
        m_forest.push_back(trainedRDT);
    }
    qDebug()<< "Forest Size : " << m_forest.size();
}

//padded image index must be provided
cv::Mat RandomDecisionForest::classify(int index)
{
    cv::Mat test_image = m_DS.m_testImagesVector[index];
    int nRows=test_image.rows;
    int nCols=test_image.cols;

    int probDistY  = m_params.probDistY;
    int probDistX  = m_params.probDistX;
    int labelCount = m_params.labelCount;

    //typecheck
    cv::Mat res_image = cv::Mat(nRows-2*probDistY, nCols-2*probDistX, test_image.type());
    imageinfo_ptr img_Info(new ImageInfo(" ", index));

    nRows -= probDistY;
    nCols -= probDistX;

    for(int r=probDistY; r<nRows; ++r)
    {
        for(int c=probDistX; c<nCols; ++c)
        {
            auto intensity = test_image.at<uchar>(r,c);
            pixel_ptr px(new Pixel(Coord(r,c),intensity,img_Info));
            cv::Mat probHist = cv::Mat::zeros(1, labelCount, cv::DataType<float>::type);
            auto nForest = m_forest.size();
            for(unsigned int i=0; i<nForest; ++i)
            {
                node_ptr leaf = m_forest[i]->getLeafNode(m_DS, px, 0);
                probHist += leaf->m_hist;
            }
            // Type check of label
            auto label = getMaxLikelihoodIndex(probHist);
            res_image.at<uchar>(r-probDistY,c-probDistX) = label;
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
        cv::Mat votes = cv::Mat::zeros(1, m_params.labelCount, CV_32FC1);
        cv::Mat label_image = classify(i);

        if(votes.cols != m_params.labelCount)
        {
            std::cerr<<"Assertion Failed BRO!  Number of labels mismatch"<< std::endl;
            return;
        }
        getImageLabelVotes(label_image, votes);
        std::cout<<votes<<std::endl;
        int label = getMaxLikelihoodIndex(votes);
        char predicted_label = char('a' + label);
        classify_res.push_back(QString(predicted_label));
        emit classifiedImageAs(i+1, predicted_label);
        votes.release();
    }
    m_accuracy = Util::calculateAccuracy(m_DS.m_testlabels, classify_res);
    emit resultPercentage(m_accuracy);
}

