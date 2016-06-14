#ifndef CPV_READER
#define CPV_READER


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QDirIterator>

#include <dirent.h>
#include <iostream>

#include "MNIST.h"

class Reader
{
  public:
    void readFromTo(std::string filename, std::vector<QString> &imgName);
    void findImages(QString baseDir, QString query,
                    std::vector<QString> &foundImages, std::vector<QString> &labels);
    void findImages(QString baseDir, QString query,
                    std::vector<QString> &foundImages);
    void MNISTReader(QString filepath, std::vector<cv::Mat> &imagesVector,
                     std::vector<cv::Mat> &trainImagesVector, std::vector<int> &testLabels,
                     std::vector<int> &trainLabels);
    static void readTextFiles(QString baseDir, std::vector<QString> &foundText);
    static void readTextFiles(QString baseDir, QString query,
                              std::vector<QString> &foundText);
  private:
    DIR *m_dir;
};

#endif
