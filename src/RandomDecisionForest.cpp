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
    QString path = m_dir + "/"+ px->imgInfo->m_label + "/" + px->imgInfo->m_label + "_"
            + QString::number(px->imgInfo->m_sampleId)  + ".jpg";
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
        //rdt_ptr trainedRDT(new RandomDecisionTree(rdf_ptr(this)));
        rdt_ptr trainedRDT(new RandomDecisionTree(this));
        trainedRDT->setProbeDistanceX(m_params.probDistX);
        trainedRDT->setProbeDistanceY(m_params.probDistY);
        trainedRDT->setMaxDepth(m_params.maxDepth);
        trainedRDT->setMinimumLeafPixelCount(m_params.minLeafPixels);
        qDebug()<< "Train..." ;
        trainedRDT->train();
        //trainedRDT->saveTree();
        m_forest.push_back(trainedRDT);
    }
    qDebug()<< "Forest Size : " << m_forest.size();
}

//padded image index must be provided
cv::Mat RandomDecisionForest::getLayeredHist(cv::Mat test_image, int index, QVector<quint32> &fgPxNumberPerCol)
{

    int nRows=test_image.rows;
    int nCols=test_image.cols;
    int probDistY  = m_params.probDistY;
    int probDistX  = m_params.probDistX;
    int labelCount = m_params.labelCount;

    fgPxNumberPerCol = QVector<quint32>(nCols);

    //typecheck
    cv::Mat layeredHist = cv::Mat(nRows-2*probDistY, (nCols-2*probDistX)*labelCount, CV_32FC1);

    imageinfo_ptr img_Info(new ImageInfo(" ", index));

    int rangeRows = nRows - probDistY;
    int rangeCols = nCols - probDistX;

    for(int c=probDistX; c<rangeCols; ++c)
    {
        int col_index = (c-probDistX)*labelCount;
        int fgPxCount = 0;
        for(int r=probDistY; r<rangeRows; ++r)
        {

            cv::Mat probHist = cv::Mat::zeros(1, labelCount, cv::DataType<float>::type);
            auto intensity = test_image.at<uchar>(r,c);

            if (intensity == 0)
            {
                placeHistogram(layeredHist, probHist, r-probDistY, col_index);
            }

            else
            {
                ++fgPxCount;
                pixel_ptr px(new Pixel(Coord(r,c),intensity,img_Info));
                auto nForest = m_forest.size();
                for(unsigned int i=0; i<nForest; ++i)
                {
                    node_ptr leaf = m_forest[i]->getLeafNode(m_DS, px, 0);
                    probHist += leaf->m_hist;

                }
                //Normalize the Histrograms
                float sum = cv::sum(probHist)[0];
                if( sum != 0 )
                    probHist /= sum;
                placeHistogram(layeredHist, probHist, r-probDistY, col_index);


            }
        }
        fgPxNumberPerCol[col_index/labelCount] = fgPxCount;
    }

    return layeredHist;
}

void RandomDecisionForest::placeHistogram(cv::Mat &output, const cv::Mat &pixelHist, int pos_row, int pos_col)
{
    auto range = pos_col+m_params.labelCount;

    for (int i = pos_col; i < range ; ++i)
    {
        output.at<float>(pos_row,i) = pixelHist.at<float>(i-pos_col);
    }
}

// confidenceMat : LabelCount x testImage.rows : each row displays likelooh for each column of a label.
cv::Mat RandomDecisionForest::createLetterConfidenceMatrix(const cv::Mat &layeredHist, const QVector<quint32> &fgPxNumberPerCol)
{
    int labelcount = m_params.labelCount;
    int layered_Cols = layeredHist.cols;
    int confmat_cols = layered_Cols/labelcount;

    cv::Mat confidenceMat(labelcount, confmat_cols, CV_32FC1);
    int i_row;
    int i_col;

    for (int i = 0; i < layered_Cols; ++i)
    {
        i_row = i%labelcount;
        i_col = i/labelcount;
        confidenceMat.at<float>(i_row, i_col) = cv::sum(layeredHist.col(i))[0];
    }

    Util::averageLHofCol(confidenceMat,fgPxNumberPerCol);
//    Util::plot(confidenceMat.row(0),m_parent);


    return confidenceMat;
}


void RandomDecisionForest::test()
{
    int nImages = m_DS.m_testImagesVector.size();
    qDebug() << "Number of Test images:" << QString::number(nImages);

    for(auto i=0; i<nImages; ++i)
    {
        QVector<quint32> fgPxNumberPerCol;
        cv::Mat layeredImage = getLayeredHist(m_DS.m_testImagesVector[i], i, fgPxNumberPerCol);
        cv::Mat confidenceMat =  createLetterConfidenceMatrix(layeredImage, fgPxNumberPerCol);

//        QString test = "Hello";
//        float acc = 0;
//        Util::getWordWithConfidance(confidenceMat,26,test,acc);
//        Util::plot(confidenceMat.row(23), m_parent, "x");
//        Util::plot(confidenceMat.row(24), m_parent, "y");
//        Util::plot(confidenceMat.row(25), m_parent, "z");
    }
//    m_accuracy = Util::calculateAccuracy(m_DS.m_testlabels, classify_res);
//    emit resultPercentage(m_accuracy);
}

