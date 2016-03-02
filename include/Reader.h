#ifndef CPV_READER
#define CPV_READER


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "QString"
#include <QDir>
#include<QDebug>
#include "QApplication"
#include<QDirIterator>

#include <dirent.h>
#include <iostream>


using namespace cv;
using namespace std;

class Reader{
public:
    void readFromTo(string filename, vector<QString> &imgName);
    void findImages(QString baseDir, QString query, vector<QString> &foundImages);
private:
    DIR *m_dir;
};

#endif
