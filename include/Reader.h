#ifndef CPV_READER
#define CPV_READER


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "QString"
#include <QDir>
#include <QDebug>
#include "QApplication"
#include <QDirIterator>

#include <dirent.h>
#include <iostream>





class Reader{
public:
    void readFromTo(std::string filename, std::vector<QString> &imgName);
    void findImages(QString baseDir, QString query, std::vector<QString> &foundImages);
private:
    DIR *m_dir;
};

#endif
