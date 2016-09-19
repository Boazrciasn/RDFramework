#include "HOGExtactor.h"
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

HOGExtactor::HOGExtactor()
{
    QString dir = QFileDialog::getExistingDirectory(nullptr, QObject::tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    getTrainingData(dir);
    extractHOG();

//    int maxComponents = 2;
//    cv::abspca_analysis(m_trainDataDescriptors, Mat(), CV_PCA_DATA_AS_ROW,maxComponents);
}

void HOGExtactor::getTrainingData(QString baseDir)
{
    QDirIterator itFile(baseDir, QStringList() << "*.jpg" << "*.jpeg" << "*.png", QDir::Files,
                        QDirIterator::Subdirectories);
    while(itFile.hasNext())
    {
        itFile.next();
        m_trainDataFiles.append(itFile.filePath().toStdString());
    }
}

void HOGExtactor::extractHOG()
{
    doForAll<float>(m_trainDataFiles, m_trainDataDescriptors,
                    [&](std::string path)
    {
        cv::Mat grayImg = cv::imread(path,CV_LOAD_IMAGE_GRAYSCALE);
        std::vector<cv::Point> positions;
        std::vector<float> descriptor;
        positions.push_back(cv::Point(grayImg.cols / 2, grayImg.rows / 2));
        m_hog.compute(grayImg,descriptor,cv::Size(32,32),cv::Size(0,0),positions);
        return descriptor;
    });
}
