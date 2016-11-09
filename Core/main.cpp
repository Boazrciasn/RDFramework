#include "precompiled.h"

#include <vector>

#include "Core/MainWindowGui.h"
#include "util/Reader.h"
#include "rdf/RandomDecisionForest.h"



#include "Util.h"
#include <opencv2/objdetect.hpp>
#include <opencv2/ml.hpp>

int main(int argc, char *argv[])
{
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
    QString MNIST_path = "/Users/barisozcan/Documents/MNIST_dataset/blahblah.xml";
    std::vector<cv::Mat> test;
    Reader reader;
    QApplication app(argc, argv);
    MainWindowGui w;
    w.show();
    reader.readImages(MNIST_path, test, Type_MNIST);
    return app.exec();
}
