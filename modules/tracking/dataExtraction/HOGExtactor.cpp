#include "precompiled.h"

#include "HOGExtactor.h"
#include <QFileDialog>
#include "Util.h"

HOGExtactor::HOGExtactor()
{
    QString dir = QFileDialog::getExistingDirectory(nullptr, QObject::tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    getTrainingData(dir);
    extractHOG();
    //    int maxComponents = 2;
    cv::Mat_<float> data;
    for (auto desc : m_trainDataDescriptors)
        data.push_back((cv::Mat_<float>(desc)).t());
//    Util::writeMatToFile(data, "../negDes.txt");
    //    cv::PCA pca_analysis(data, cv::Mat(), CV_PCA_DATA_AS_ROW,maxComponents);
    //    cv::Mat proj = data*pca_analysis.eigenvectors.t();
    //    Util::writeMatToFile(proj,"../neg.txt");
        cv::FileStorage file("../posDes2.yml", cv::FileStorage::WRITE);
        file << "posDes" << data;
        file.release();
}

void HOGExtactor::getTrainingData(QString baseDir)
{
    QDirIterator itFile(baseDir, QStringList() << "*.jpg" << "*.jpeg" << "*.png", QDir::Files,
                        QDirIterator::Subdirectories);
    while (itFile.hasNext())
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
        cv::Mat grayImg = cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE);
        std::vector<cv::Point> positions;
        std::vector<float> descriptor;
        positions.push_back(cv::Point(grayImg.cols / 2, grayImg.rows / 2));
        m_hog.compute(grayImg, descriptor, cv::Size(64, 128), cv::Size(16, 16), positions);
        return descriptor;
    });
}
