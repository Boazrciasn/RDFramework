#ifndef CPV_READER
#define CPV_READER


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "QString"


#include <dirent.h>
#include <iostream>


using namespace cv;
using namespace std;

class Reader{
public:
    void readFromTo(string filename, vector<QString> &imgName);
private:
    DIR *m_dir;
};

#endif
