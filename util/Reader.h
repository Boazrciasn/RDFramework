#ifndef CPV_READER
#define CPV_READER


#include <QString>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QDirIterator>

#include <dirent.h>
#include <iostream>

#include "ReaderMNIST.h"

class Reader
{
  public:
    static void findImages(QString baseDir, QString query, std::vector<QString> &foundImages, std::vector<QString> &labels);
    static void findImages(QString baseDir, QString query, std::vector<QString> &foundImages);
    static void readTextFiles(QString baseDir, std::vector<QString> &foundText);
    static void readTextFiles(QString baseDir, QString query, std::vector<QString> &foundText);
    static void findImages(QString baseDir, std::vector<QString> &foundImages);
    static void readImages(QString dir);
    inline void setReadDir(QString dir) { m_dir = dir; }

  private:
    QString m_dir;

};

#endif
