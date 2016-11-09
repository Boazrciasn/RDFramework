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
    Type_Standard = 0,
    Type_MNIST = 1
};

class Reader
{
  public:
    static void findImages(QString baseDir, QString query, std::vector<QString> &foundImages, std::vector<QString> &labels);
    static void findImages(QString baseDir, QString query, std::vector<QString> &foundImages);
    static void readTextFiles(QString baseDir, std::vector<QString> &foundText);
    static void readTextFiles(QString baseDir, QString query, std::vector<QString> &foundText);
    static void readImages(QString dir, std::vector<cv::Mat> &foundImages, int flags);
    static void readImages(QString dir, std::vector<QString> &foundImages);

    inline void setReadDir(QString dir) { m_dir = dir; }

  private:
    QString m_dir;

};

#endif
