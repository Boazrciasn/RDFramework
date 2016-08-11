#include "precompiled.h"
#include <ctime>

#include "RandomDecisionForest.h"
#include "ocr/Reader.h"
#include "Util.h"
#include "ocr/TextRegionDetector.h"
//#include <omp.h>

// histogram normalize ?
// getLeafNode and Test  needs rework
// given the directory of the all samples
// read subsampled part of the images into pixel cloud
void RandomDecisionForest::readTrainingImageFiles()
{
    // TODO :  make applicable to both MNIST and image folders
    std::vector<QString> fNames;
    auto *reader = new Reader();
    reader->findImages(m_dir, "", fNames, m_DS.m_trainlabels);
    m_numOfLetters = fNames.size();
    qDebug() << "NO OF LETTERS : " << m_numOfLetters;
    for (auto filePath : fNames)
    {
        cv::Mat image = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
        cv::copyMakeBorder(image, image, m_params.probDistY, m_params.probDistY,
                           m_params.probDistX, m_params.probDistX, cv::BORDER_CONSTANT);
        m_DS.m_trainImagesVector.push_back(image);
    }
    qDebug() << "No of IMAGES : " << m_DS.m_trainImagesVector.size() << " NO of Fnames" << m_numOfLetters <<
             "Images Vector Size : " << m_DS.m_trainImagesVector.size();
    fNames.clear();
}

void RandomDecisionForest::readTestImageFiles()
{
    // TODO :  make applicable to both MNIST and image folders
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
        cv::copyMakeBorder(image, image, m_params.probDistY, m_params.probDistY,
                           m_params.probDistX, m_params.probDistX, cv::BORDER_CONSTANT);
        m_DS.m_testImagesVector.push_back(image);
    }
    qDebug() << "No of test IMAGES : " << m_DS.m_testImagesVector.size();
    fNames.clear();
}

void RandomDecisionForest::readAndIdentifyWords()
{
    m_dir = m_params.testDir;
    std::vector<QString> fNames;
    auto *reader = new Reader();
    reader->findImages(m_dir, "", fNames, m_DS.m_testlabels);
    // source file for average values
    QString lineOffsetFile = "./combined.txt";
    QFile input(lineOffsetFile);
    if(!input.open(QIODevice::ReadOnly))
        std::cout <<
                  "RandomDecisionForest::readAndIdentifyWords failed to open file! \n";
    QString formattedOutputFile = "./formatted_output.txt";
    QFile output(formattedOutputFile);
    if(!output.open(QIODevice::WriteOnly))
        std::cout <<
                  "RandomDecisionForest::readAndIdentifyWords failed to open file! \n";
    auto lineNo = 0;
    for (auto filePath : fNames)
    {
        //read offset line
        QString offsetLine = input.readLine();
        QStringList myStringList = offsetLine.split(' ');
        int offsetX = myStringList[2].split('+')[1].toInt();
        int offsetY = myStringList[3].split('+')[1].toInt();
        m_DS.m_testImagesVector.clear();
        cv::Mat image = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
        // Call word extractor
        QVector<QRect> wordsRoi = TextRegionDetector::detectWordsFromLine(image,
                                                                          m_parent);
        //pad image and save to vector
        cv::copyMakeBorder(image, image, m_params.probDistY, m_params.probDistY,
                           m_params.probDistX, m_params.probDistX, cv::BORDER_CONSTANT);
        m_DS.m_testImagesVector.push_back(image);
        //Note: index 0 because there is only one image at each step
        QVector<quint32> fgPxNumberPerCol;
        cv::Mat layeredImage = getLayeredHist(m_DS.m_testImagesVector[0], 0,
                                              fgPxNumberPerCol);
        for(QRect wordRoi : wordsRoi)
        {
            cv::Rect layeredWordRoi = cv::Rect(wordRoi.x(), wordRoi.y(),
                                               wordRoi.width() * m_params.labelCount, wordRoi.height());
            // Crop layered words from layared image using extracted rects
            cv::Mat layeredWordRoiMat =  layeredImage(layeredWordRoi);
            cv::Mat confidenceMat =  createLetterConfidenceMatrix(layeredWordRoiMat,
                                                                  fgPxNumberPerCol);
            //FIXME : Nekruz baba top sende
            //            Util::plot(confidenceMat.row(23), m_parent, "x");
            QString wordDetected = "baris";
            float conf = 0;
            //Util::getWordWithConfidance(confidenceMat,26,wordDetected,conf);
            //            Util::plot(confidenceMat.row(23), m_parent, "x");
            //save obtained result
            output.write(wordDetected.toStdString().c_str());
            output.write(" " + QByteArray::number(conf));
            output.write(" " + QByteArray::number(++lineNo));
            output.write(":" + QByteArray::number(wordRoi.width()));
            output.write("X" + QByteArray::number(wordRoi.height()));
            output.write("+" + QByteArray::number(offsetX + wordRoi.x()));
            output.write("+" + QByteArray::number(offsetY + wordRoi.y()));
            output.write("\n");
        }
    }
    output.close();
    input.close();
    fNames.clear();
}


void RandomDecisionForest::searchWords(QString query, int queryId)
{
    // source file for average values
    QString formattedInputFile = "./formatted_output.txt";
    QFile input(formattedInputFile);
    if(!input.open(QIODevice::ReadOnly))
        std::cout << "RandomDecisionForest::searchWords failed to open file! \n";
    QString endResultFile = "./end_result.txt";
    QFile output(endResultFile);
    if(!output.open(QIODevice::WriteOnly))
        std::cout << "RandomDecisionForest::searchWords failed to open file! \n";
    QStringList queryList = query.split(' ');
    //lineid, linecontent
    QMultiHash<int, QString> hash;
    auto segmentNo = 1;
    while (!input.atEnd())
    {
        QString offsetLine = "";
        int lineid;
        int segmentEnd = segmentNo + 5;
        do
        {
            //read offset line
            offsetLine = input.readLine();
            QStringList myStringList = offsetLine.split(' ');
            lineid = myStringList[2].split(':')[0].toInt();
            QString word = myStringList[0];
            if(lineid <= segmentEnd)
                hash.insert(lineid, offsetLine);
        }
        while(lineid <= segmentEnd && !input.atEnd());
        //TODO make ordered.
        //check for matches
        for(QString search : queryList)
        {
            for(int line_id = segmentNo; line_id <= segmentEnd; ++line_id)
            {
                QMultiHash<int, QString>::const_iterator i = hash.find(line_id);
                while (i != hash.end() && i.key() == line_id)
                {
                    QString details = i.value();
                    QStringList myStringList = details.split(' ');
                    QString word = myStringList[0];
                    if(word == search)
                    {
                        QString conf_bbox = " " + myStringList[1] + " " + myStringList[2];
                        output.write(QByteArray::number(queryId));
                        output.write(" " + QByteArray::number(segmentNo));
                        output.write(conf_bbox.toStdString().c_str());
                    }
                    ++i;
                }
            }
        }
        hash.remove(segmentNo++);
        hash.insert(lineid, offsetLine);
    }
    hash.clear();
    output.close();
    input.close();
}



//FOR TEST PURPOSES ONLY : given the image path, fills the vector with in the pixels of the image,
//img_Info : label of  test image & id of the image inside vector(optional)
void RandomDecisionForest::imageToPixels(std::vector<pixel_ptr> &res,
                                         const cv::Mat &image , imageinfo_ptr img_inf )
{
    int nRows = image.rows;
    int nCols = image.cols;
    for(int i = 0; i < nRows; ++i)
    {
        for(int j = 0; j < nCols; ++j)
        {
            auto intensity = image.at<uchar>(i, j);
            pixel_ptr px(new Pixel(Coord(i, j), intensity, img_inf));
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
    QString path = m_dir + "/" + px->imgInfo->m_label + "/" + px->imgInfo->m_label +
                   "_"
                   + QString::number(px->imgInfo->m_sampleId)  + ".jpg";
    //qDebug()<<"IMAGE :"<< path;
    return cv::imread(path.toStdString());
}

cv::Mat RandomDecisionForest::colorCoder(const cv::Mat &labelImage,
                                         const cv::Mat &InputImage)
{
    int nRows = InputImage.rows;
    int nCols = InputImage.cols;
    int labelCount = m_params.labelCount;
    cv::Mat color(nRows, nCols, CV_8UC1);
    //create color code Mat accoridng to of each pixel.
    for(int i = 0; i < nRows; ++i)
    {
        for(int j = 0; j < nCols; ++j)
        {
            auto label = labelImage.at<double>(i, j);
            color.at<double>(i, j) = (label / labelCount) * 255;
        }
    }
    //create 3 channel image with 2 channels with same intensity values of cv::Mat color :
    std::vector<cv::Mat> Image_channels(3);
    Image_channels.at(0) = InputImage;
    Image_channels.at(1) = color;
    Image_channels.at(2) = color;
    cv::Mat codedImage;
    cv::merge(Image_channels, codedImage);
    return codedImage;
}


void RandomDecisionForest::trainForest()
{
    double cpu_time;
    //#pragma omp parallel for num_threads(8)
    for (int i = 0; i < m_params.nTrees; ++i)
    {
        clock_t start = clock();
        //#pragma omp critical (DEBUG)
        //        {
        qDebug() << "Tree number " << QString::number(i + 1) << "is being trained" ;
        //        }
        //rdt_ptr trainedRDT(new RandomDecisionTree(rdf_ptr(this)));
        rdt_ptr trainedRDT(new RandomDecisionTree(this));
        trainedRDT->setProbeDistanceX(m_params.probDistX);
        trainedRDT->setProbeDistanceY(m_params.probDistY);
        trainedRDT->setMaxDepth(m_params.maxDepth);
        trainedRDT->setMinimumLeafPixelCount(m_params.minLeafPixels);
        //#pragma omp critical (DEBUG)
        //        {
        qDebug() << "Train..." ;
        //        }
        trainedRDT->train();
        //trainedRDT->saveTree();
        cpu_time = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
        //#pragma omp critical (PUSH_FOREST)
        //        {
        m_forest.push_back(trainedRDT);
        qDebug() << " Train time of the current Tree : " << cpu_time;
        //        }
    }
    qDebug() << "Forest Size : " << m_forest.size();
}

//padded image index must be provided
cv::Mat RandomDecisionForest::getLayeredHist(cv::Mat test_image, int index,
                                             QVector<quint32> &fgPxNumberPerCol)
{
    int nRows = test_image.rows;
    int nCols = test_image.cols;
    int probDistY  = m_params.probDistY;
    int probDistX  = m_params.probDistX;
    int labelCount = m_params.labelCount;
    fgPxNumberPerCol = QVector<quint32>(nCols);
    //typecheck
    cv::Mat layeredHist = cv::Mat(nRows - 2 * probDistY,
                                  (nCols - 2 * probDistX) * labelCount, CV_32FC1);
    imageinfo_ptr img_Info(new ImageInfo(" ", index));
    int rangeRows = nRows - probDistY;
    int rangeCols = nCols - probDistX;
    for(int c = probDistX; c < rangeCols; ++c)
    {
        int col_index = (c - probDistX) * labelCount;
        int fgPxCount = 0;
        for(int r = probDistY; r < rangeRows; ++r)
        {
            cv::Mat probHist = cv::Mat::zeros(1, labelCount, cv::DataType<float>::type);
            auto intensity = test_image.at<uchar>(r, c);
            if (intensity == 0)
            {
                placeHistogram(layeredHist, probHist, r - probDistY, col_index);
            }
            else
            {
                ++fgPxCount;
                pixel_ptr px(new Pixel(Coord(r, c), intensity, img_Info));
                auto nForest = m_forest.size();
                for(unsigned int i = 0; i < nForest; ++i)
                {
                    node_ptr leaf = m_forest[i]->getLeafNode(m_DS, px, 0);
                    probHist += leaf->m_hist;
                }
                //Normalize the Histrograms
                float sum = cv::sum(probHist)[0];
                if( sum != 0 )
                    probHist /= sum;
                placeHistogram(layeredHist, probHist, r - probDistY, col_index);
            }
        }
        fgPxNumberPerCol[col_index / labelCount] = fgPxCount;
    }
    return layeredHist;
}

void RandomDecisionForest::placeHistogram(cv::Mat &output,
                                          const cv::Mat &pixelHist, int pos_row, int pos_col)
{
    auto range = pos_col + m_params.labelCount;
    for (int i = pos_col; i < range ; ++i)
    {
        output.at<float>(pos_row, i) = pixelHist.at<float>(i - pos_col);
    }
}

// confidenceMat : LabelCount x testImage.rows : each row displays likelooh for each column of a label.
cv::Mat_<float> RandomDecisionForest::createLetterConfidenceMatrix(const cv::Mat &layeredHist,
                                                                   const QVector<quint32> &fgPxNumberPerCol)
{
    int labelcount = m_params.labelCount;
    int layered_Cols = layeredHist.cols;
    int confmat_cols = layered_Cols / labelcount;
    cv::Mat_<float> confidenceMat(labelcount, confmat_cols);
    int i_row;
    int i_col;
    for (int i = 0; i < layered_Cols; ++i)
    {
        i_row = i % labelcount;
        i_col = i / labelcount;
        confidenceMat.at<float>(i_row, i_col) = cv::sum(layeredHist.col(i))[0];
    }
    Util::averageLHofCol(confidenceMat, fgPxNumberPerCol);
    //    Util::plot(confidenceMat.row(0),m_parent);
    return confidenceMat;
}


void RandomDecisionForest::test()
{
    int nImages = m_DS.m_testImagesVector.size();
    qDebug() << "Number of Test images:" << QString::number(nImages);
    for(auto i = 0; i < nImages; ++i)
    {
        QVector<quint32> fgPxNumberPerCol;
        cv::Mat layeredImage = getLayeredHist(m_DS.m_testImagesVector[i], i,
                                              fgPxNumberPerCol);
        cv::Mat_<float> confidenceMat = createLetterConfidenceMatrix(layeredImage, fgPxNumberPerCol);
        //        std::cout<<confidenceMat.row(0)<<std::endl;
        QString word = "Hello";
        float conf = 0;
        //        Util::plot(confidenceMat.row('n'-'a'), m_parent, "n");
        Util::getWordWithConfidence(confidenceMat, 26, word, conf);
        qDebug() << "Word extracted & conf: " << word << "  " << 100 * conf;
    }
    //    m_accuracy = Util::calculateAccuracy(m_DS.m_testlabels, classify_res);
    //    emit resultPercentage(m_accuracy);
}

