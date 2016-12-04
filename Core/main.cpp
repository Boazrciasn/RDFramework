#include "precompiled.h"

#include <vector>

#include "Core/MainWindowGui.h"

#include <opencv2/objdetect.hpp>
#include <opencv2/ml.hpp>
using namespace cv;

cv::Mat get_hogdescriptor_visu(const cv::Mat& color_origImg, std::vector<float>& descriptorValues, const cv::Size & size );

int main(int argc, char *argv[])
{
    //    cv::Mat_<float> test = cv::Mat_<float>::zeros(1,10);
    //    test(cv::Range(0,1),cv::Range(2,5)) += cv::Mat_<float>::ones(1,3);
    //    test(cv::Range(0,1),cv::Range(2,5)) += cv::Mat_<float>::ones(1,3);
    //    std::cout << "test: " << test << std::endl;

    //    cv::Mat_<float> tmpProbHist = test(cv::Range(0,1),cv::Range(2,5));
    //    tmpProbHist(2) ++;
    //    std::cout << "tmpProbHist: " << tmpProbHist << std::endl;

    //    double max;
    //    cv::Point max_loc;
    //    cv::minMaxLoc(tmpProbHist, NULL, &max, NULL, &max_loc);
    //    std::cout << "max: " << max << std::endl;
    //    std::cout << "max_loc: " << max_loc.x << std::endl;
    // TODO: Baris Fix case sensitivity
    //BOUNDING BOX TEST :
    //    cv::Mat src_gray;
    //    cv::Mat threshold_output;
    //    cv::Mat src = cv::imread("/Users/barisozcan/Documents/Development/bbtest.jpg", 1);
    //    cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
    //    cv::blur(src_gray, src_gray, cv::Size(3, 3));
    //    threshold(src_gray, threshold_output, 100, 255, cv::THRESH_BINARY);
    //    std::vector<cv::Rect> boundingboxes;
    //    boundingboxes = Util::calculateBoundingBoxRect(threshold_output, 500);
    //    Util::drawBoundingBox(threshold_output, boundingboxes);
    //    cv::imwrite("bbThreshold.jpg", threshold_output);
    //    QString file = "/home/neko/Desktop/lastSession.txt";
    //    QFile  *m_saveFile = new QFile(file);
    //    bool fileExists = m_saveFile->exists();
    //    m_saveFile->open(QIODevice::ReadWrite);
    //    if (fileExists)
    //    {
    //        // Read header and set values
    //        qDebug()<<"file does not exist! " << fileExists;
    //        int c = 0;
    //        do
    //        {
    //            qDebug()<< m_saveFile->readLine();
    //            c++;
    //        }while(m_saveFile->canReadLine() & (c < 4));
    //        m_saveFile->write("Testing");
    //        m_saveFile->write(" \n");
    //    }
    //    else
    //    {
    //        qDebug()<<"open success!";
    //        // set initial values and create header
    //        int m_avgWidth = 1;
    //        int m_avgHight = 2;
    //        int m_sampleCount = 100;
    //        m_saveFile->write(QByteArray::number(m_avgWidth));
    //        m_saveFile->write(" \n");
    //        m_saveFile->write(QByteArray::number(m_avgHight));
    //        m_saveFile->write(" \n");
    //        m_saveFile->write(QByteArray::number(m_sampleCount));
    //        m_saveFile->write(" \n");
    //        m_saveFile->write(file.toUtf8());
    //        m_saveFile->write(" \n");
    //    }
    //    m_saveFile->close();
    //     MNIST EXTRACTOR :
    //        QString MNIST_path = "/Users/barisozcan/Documents/MNIST_dataset/";
    //        QString destdir = "/Users/barisozcan/";
    //        MNIST mnist(MNIST_path);
    //        mnist.MNISTReader();
    //        qDebug() << "no of test images :" << mnist.m_testImagesVector->size();
    //        qDebug() << "no of train images :" << mnist.m_trainImagesVector->size();
    //        qDebug() << "test label count : " << mnist.m_testLabels->size();
    //        qDebug() << "train label count : " << mnist.m_trainLabels->size();
    //        mnist.extractDataSet(destdir);
    //    Animal *animal_pntr = new Animal();
    //    animal_pntr->execute();
    //    Cat<float> *cat_pntr1 = new Cat<float>(15.0f);
    //    Dog<QString> *dog_pntr1 = new Dog<QString>("How Many");
    //    animal_pntr = cat_pntr1;
    //    animal_pntr->execute();
    //    animal_pntr = dog_pntr1;
    //    animal_pntr->execute();
    /************** Testing trained data **************/
    //    cv::Mat posData, negData, allData, labels;
    //    cv::FileStorage file("../posDes.yml", cv::FileStorage::READ);
    //    file["data"] >> posData;
    //    file.release();
    //    file.open("../negDes.yml",cv::FileStorage::READ);
    //    file["data"] >> negData;
    //    file.release();
    //    allData.push_back(posData);
    //    allData.push_back(negData);
    //    labels.push_back(cv::Mat::ones(posData.rows,1,CV_32SC1));
    //    labels.push_back(cv::Mat::zeros(negData.rows,1,CV_32SC1));
    //    cv::HOGDescriptor *hog = new cv::HOGDescriptor();
    //    cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
    //    svm->setKernel(cv::ml::SVM::LINEAR);
    //    svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 10000, 1e-6));
    //    svm->train( allData , cv::ml::ROW_SAMPLE , labels );
    //    QString dir = QFileDialog::getExistingDirectory(nullptr, QObject::tr("Open Directory"),
    //                                                    "/home",
    //                                                    QFileDialog::ShowDirsOnly
    //                                                    | QFileDialog::DontResolveSymlinks);
    //    std::vector<QString> foundImages;
    //    Reader::findImages(dir, foundImages);
    //    float rate{};
    //    for (auto imgPath : foundImages)
    //    {
    //        cv::Mat grayImg = cv::imread(imgPath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
    ////        cv::imshow("input", grayImg);
    ////        cv::waitKey();
    //        cv::resize(grayImg,grayImg,cv::Size(64,128));
    //        std::vector<cv::Point> positions;
    //        std::vector<float> descriptor;
    //        hog->compute(grayImg, descriptor, cv::Size(128, 64), cv::Size(16, 16), positions);
    //        cv::Mat_<float> desc(descriptor);
    //        float decision = svm->predict(desc.t(),cv::noArray(), cv::ml::StatModel::RAW_OUTPUT);
    //        rate += decision;
    //        float confidence = 1.0 / (1.0 + exp(-decision));
    //        std::cout << "distBhat " << " " << decision << " confidence: " << confidence << "\n"; //<<" confidence: " <<confidence <<std::endl;
    //    }
    //    std::cout << "Rate: " << 100*rate/foundImages.size()<<std::endl;
    //    Animal *animal_pntr = new Animal();
    //    animal_pntr->execute();
    //    Cat<float> *cat_pntr1 = new Cat<float>(15.0f);
    //    Dog<QString> *dog_pntr1 = new Dog<QString>("How Many");
    //    animal_pntr = cat_pntr1;
    //    animal_pntr->execute();
    //    animal_pntr = dog_pntr1;
    //    animal_pntr->execute();
    //    /************** Testing trained data **************/
    //    cv::Mat posData, negData, allData, labels;
    //    cv::FileStorage file("../posDes2.yml", cv::FileStorage::READ);
    //    file["posDes"] >> posData;
    //    file.release();
    //    file.open("../negDes2.yml",cv::FileStorage::READ);
    //    file["negDes"] >> negData;
    //    file.release();
    //    allData.push_back(posData);
    //    allData.push_back(negData);
    //    labels.push_back(cv::Mat::ones(posData.rows,1,CV_32SC1));
    //    labels.push_back(-1*cv::Mat::ones(negData.rows,1,CV_32SC1));
    //    cv::HOGDescriptor *hog = new cv::HOGDescriptor();
    //    cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
    //    svm->setType(cv::ml::SVM::C_SVC);
    //    svm->setKernel(cv::ml::SVM::LINEAR);
    //    svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 10000, 1e-6));
    //    svm->train( allData , cv::ml::ROW_SAMPLE , labels );
    //    QString dir = "/home/neko/Desktop/INRIAPerson/test_64x128_H96/pos/";
    //    std::vector<QString> foundImages;
    //    Reader::findImages(dir, foundImages);
    //    float rate{};
    //    for (auto imgPath : foundImages)
    //    {
    //        cv::Mat grayImg = cv::imread(imgPath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
    ////        cv::imshow("input", grayImg);
    ////        cv::waitKey();
    //        std::vector<cv::Point> positions;
    //        std::vector<float> descriptor;
    //        positions.push_back(cv::Point(grayImg.cols / 2, grayImg.rows / 2));
    //        hog->compute(grayImg, descriptor, cv::Size(64, 128), cv::Size(16, 16), positions);
    //        cv::Mat_<float> desc(descriptor);
    //        float decision = svm->predict(desc.t()); //, cv::noArray(), cv::ml::StatModel::RAW_OUTPUT);
    //        rate += decision;
    ////        float confidence = 1.0 / (1.0 + exp(decision));
    //        std::cout << "distBhat " << " " << decision << "\n"; //<<" confidence: " <<confidence <<std::endl;
    //    }
    //    std::cout << "Rate: " << -100*rate/foundImages.size()<<std::endl;

    cv::HOGDescriptor hog;
    cv::Mat img = cv::imread("/home/neko/Desktop/trackingData/INRIAPerson/70X134H96/Test/pos/crop001501a.png", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat grayImg = img(cv::Range(0,128),cv::Range(0,64));


    std::vector<float> des1;
    std::vector<float> des2;
    hog.compute(img, des1);
    hog.compute(grayImg, des2,cv::Size( 8, 8 ), cv::Size( 0, 0 ));
    Mat visu = get_hogdescriptor_visu(grayImg,des2,Size(64,128));
    imshow("visu", visu);
    waitKey();

    qDebug()<<img.rows << " " << img.cols << " " << des1.size();
    qDebug()<<grayImg.rows << " " << grayImg.cols << " " << des2.size();

    //    for(int i = 0; i < 30; ++i)
    //        qDebug()<< (des1[i]-des2[i]);

    cv::imshow("img", grayImg);
    cv::waitKey();

    QApplication app(argc, argv);
    MainWindowGui w;
    w.show();
    return app.exec();
}

cv::Mat get_hogdescriptor_visu(const cv::Mat& color_origImg, std::vector<float>& descriptorValues, const cv::Size & size )
{
    const int DIMX = size.width;
    const int DIMY = size.height;
    float zoomFac = 3;
    Mat visu;
    resize(color_origImg, visu, Size( (int)(color_origImg.cols*zoomFac), (int)(color_origImg.rows*zoomFac) ) );

    int cellSize        = 8;
    int gradientBinSize = 9;
    float radRangeForOneBin = (float)(CV_PI/(float)gradientBinSize); // dividing 180 into 9 bins, how large (in rad) is one bin?

    // prepare data structure: 9 orientation / gradient strenghts for each cell
    int cells_in_x_dir = DIMX / cellSize;
    int cells_in_y_dir = DIMY / cellSize;
    float*** gradientStrengths = new float**[cells_in_y_dir];
    int** cellUpdateCounter   = new int*[cells_in_y_dir];
    for (int y=0; y<cells_in_y_dir; y++)
    {
        gradientStrengths[y] = new float*[cells_in_x_dir];
        cellUpdateCounter[y] = new int[cells_in_x_dir];
        for (int x=0; x<cells_in_x_dir; x++)
        {
            gradientStrengths[y][x] = new float[gradientBinSize];
            cellUpdateCounter[y][x] = 0;

            for (int bin=0; bin<gradientBinSize; bin++)
                gradientStrengths[y][x][bin] = 0.0;
        }
    }

    // nr of blocks = nr of cells - 1
    // since there is a new block on each cell (overlapping blocks!) but the last one
    int blocks_in_x_dir = cells_in_x_dir - 1;
    int blocks_in_y_dir = cells_in_y_dir - 1;

    // compute gradient strengths per cell
    int descriptorDataIdx = 0;
    int cellx = 0;
    int celly = 0;

    for (int blockx=0; blockx<blocks_in_x_dir; blockx++)
    {
        for (int blocky=0; blocky<blocks_in_y_dir; blocky++)
        {
            // 4 cells per block ...
            for (int cellNr=0; cellNr<4; cellNr++)
            {
                // compute corresponding cell nr
                cellx = blockx;
                celly = blocky;
                if (cellNr==1) celly++;
                if (cellNr==2) cellx++;
                if (cellNr==3)
                {
                    cellx++;
                    celly++;
                }

                for (int bin=0; bin<gradientBinSize; bin++)
                {
                    float gradientStrength = descriptorValues[ descriptorDataIdx ];
                    descriptorDataIdx++;

                    gradientStrengths[celly][cellx][bin] += gradientStrength;

                } // for (all bins)


                // note: overlapping blocks lead to multiple updates of this sum!
                // we therefore keep track how often a cell was updated,
                // to compute average gradient strengths
                cellUpdateCounter[celly][cellx]++;

            } // for (all cells)


        } // for (all block x pos)
    } // for (all block y pos)


    // compute average gradient strengths
    for (celly=0; celly<cells_in_y_dir; celly++)
    {
        for (cellx=0; cellx<cells_in_x_dir; cellx++)
        {

            float NrUpdatesForThisCell = (float)cellUpdateCounter[celly][cellx];

            // compute average gradient strenghts for each gradient bin direction
            for (int bin=0; bin<gradientBinSize; bin++)
            {
                gradientStrengths[celly][cellx][bin] /= NrUpdatesForThisCell;
            }
        }
    }

    // draw cells
    for (celly=0; celly<cells_in_y_dir; celly++)
    {
        for (cellx=0; cellx<cells_in_x_dir; cellx++)
        {
            int drawX = cellx * cellSize;
            int drawY = celly * cellSize;

            int mx = drawX + cellSize/2;
            int my = drawY + cellSize/2;

            rectangle(visu, Point((int)(drawX*zoomFac), (int)(drawY*zoomFac)), Point((int)((drawX+cellSize)*zoomFac), (int)((drawY+cellSize)*zoomFac)), Scalar(100,100,100), 1);

            // draw in each cell all 9 gradient strengths
            for (int bin=0; bin<gradientBinSize; bin++)
            {
                float currentGradStrength = gradientStrengths[celly][cellx][bin];

                // no line to draw?
                if (currentGradStrength==0)
                    continue;

                float currRad = bin * radRangeForOneBin + radRangeForOneBin/2;

                float dirVecX = cos( currRad );
                float dirVecY = sin( currRad );
                float maxVecLen = (float)(cellSize/2.f);
                float scale = 2.5; // just a visualization scale, to see the lines better

                // compute line coordinates
                float x1 = mx - dirVecX * currentGradStrength * maxVecLen * scale;
                float y1 = my - dirVecY * currentGradStrength * maxVecLen * scale;
                float x2 = mx + dirVecX * currentGradStrength * maxVecLen * scale;
                float y2 = my + dirVecY * currentGradStrength * maxVecLen * scale;

                // draw gradient visualization
                line(visu, Point((int)(x1*zoomFac),(int)(y1*zoomFac)), Point((int)(x2*zoomFac),(int)(y2*zoomFac)), Scalar(0,255,0), 1);

            } // for (all bins)

        } // for (cellx)
    } // for (celly)


    // don't forget to free memory allocated by helper data structures!
    for (int y=0; y<cells_in_y_dir; y++)
    {
        for (int x=0; x<cells_in_x_dir; x++)
        {
            delete[] gradientStrengths[y][x];
        }
        delete[] gradientStrengths[y];
        delete[] cellUpdateCounter[y];
    }
    delete[] gradientStrengths;
    delete[] cellUpdateCounter;

    return visu;
}
