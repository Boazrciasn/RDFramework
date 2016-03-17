#ifndef CPV_RANDOM_DESICION_FOREST
#define CPV_RANDOM_DESICION_FOREST


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "QString"
#include <QDir>
#include <QDebug>
#include "QApplication"
#include <QDirIterator>
#include <include/Util.h>

#include <dirent.h>
#include <iostream>

#include <time.h>       /* time */

#define MAX_NUMITERATION_FOR_DIVISION 200
#define PIXEL_CLOUD_SIZE 1000
#define MIN_ENTROPY 0.05
#define NUM_LABELS 23
#define MAX_DEPTH 16

using namespace cv;
using namespace std;


struct Coord{
    int x,y;
    Coord(){x=0;y=0;}
    Coord(int xpos, int ypos) :
        x(xpos), y(ypos) {}
    Coord(const Coord& crd) : x(crd.x), y(crd.x) { } // user-defined copy ctor
};

struct ImageInfo
{
    char label;
    int sampleId;
    ImageInfo(char lbl, int smplId) :
        label(lbl), sampleId(smplId){}
};

struct Pixel
{
  Coord position;
  unsigned char intensity;
  ImageInfo* imgInfo;
  Pixel(Coord crd, unsigned char intnsty, ImageInfo* imgInf):
      position(crd), intensity(intnsty),imgInfo(imgInf){}
};

struct Node
{
  int taw;
  Coord teta1, teta2;
  bool isLeaf;
  int id;
  Mat hist;
};


class RandomDesicionForest{
public:
    RandomDesicionForest(int probe_dstanceX,int probe_dstanceY){
        probe_distanceX = probe_dstanceX;
        probe_distanceY = probe_dstanceY;
        initialTaw = 100;
        numOfLeaves = 0;
        numOfLetters = 0;
    }
    void readTrainingImageFiles();
    void printPixelCloud();
    void printPixel(Pixel* px);
    void generateTeta(Coord& crd);
    int generateTaw();
    float calculateEntropy(Mat& hist);
    float calculateEntropyOfVector(vector<Pixel*>& pixels);
    Mat createHistogram(vector<Pixel*>& pixels);
    bool isLeft(Pixel* p, Node& node, Mat& img);
    void divide(vector<Pixel*>& parentPixels,
                vector<Pixel*>& left, vector<Pixel*>& right, Node& parent);
    void train();
    void constructTree(vector<Node>& tree, Node& root, vector<Pixel*>& pixels);
    void tuneParameters(vector<Pixel*>& parentPixels, Node& parent);
    int pixelCloudSize();
    void printHistogram(Mat& hist);
    void printNode(Node& node);
    Mat getPixelImage(Pixel* px);
    Node getLeafNode(Pixel*px, int nodeId);
    int getLabelIndex(Mat& hist);
    int letterIndex(char px);
    void imageToPixels(vector<Pixel*>& res, QString& filePath,ImageInfo* img_inf);
    bool test(vector<Pixel*>& letterPixels, char letter);

private:
    QString dir;
    vector<Pixel*> pixelCloud;
    int probe_distanceX, probe_distanceY;
    int initialTaw;
    int numOfLetters;
    int numOfLeaves;
    int depth;
    float min_InfoGain;
    float max_InfoGain;
    vector<Node>tree;
};

#endif
