#ifndef CPV_READER
#define CPV_READER
#include "precompiled.h"

#include <QString>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QDirIterator>

#include <dirent.h>
#include <iostream>


enum
{
    Type_STD_ONE = 0,
    Type_Standard = 1,
    Type_MNIST = 2,
    Type_TME = 3
};

class Reader
{
  public:
    static void findImages(QString baseDir, QString query, std::vector<QString> &foundImages, std::vector<QString> &labels);
    static void findImages(QString baseDir, QString query, std::vector<QString> &foundImages);
    static void readTextFiles(QString baseDir, std::vector<QString> &foundText);
    static void readTextFiles(QString baseDir, QString query, std::vector<QString> &foundText);
    static void readImages(QString dir, std::vector<cv::Mat> &images, int flags);
    static void readImages(QString dir, std::vector<QString> &foundImages);
    static void readLabels(QString dir, std::vector<int> &labels, int flags);
    inline void setReadDir(QString dir) { m_dir = dir; }

  private:
    QString m_dir;

};

#endif
