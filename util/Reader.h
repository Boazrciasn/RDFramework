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

#include "ocr/ReaderMNIST.h"

class Reader
{
  public:
    static void findImages(QString baseDir, QString query,
                    std::vector<QString> &foundImages, std::vector<QString> &labels);
    static void findImages(QString baseDir, QString query,
                    std::vector<QString> &foundImages);
    static void readTextFiles(QString baseDir, std::vector<QString> &foundText);
    static void readTextFiles(QString baseDir, QString query,
                              std::vector<QString> &foundText);
    static void findImages(QString baseDir, std::vector<QString> &foundImages);
//    void dataSetReader(QString dataSetDir, );
private:
    DIR *m_dir;
};

#endif
