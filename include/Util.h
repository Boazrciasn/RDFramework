#ifndef CPV_UTIL
#define CPV_UTIL

#include "include/histogramdialog.h"
#include "include/PixelCloud.h"


inline int letterIndex(char letter)
{
    return letter-'a';
}

// creates histogram out of a pixel vector : need(?) fix after image info re-arrange.
inline cv::Mat createHistogram(PixelCloud& pixels, int labelCount)
{
    cv::Mat hist = cv::Mat::zeros(1, labelCount, cv::DataType<float>::type);
    for (Pixel *px : pixels)
    {
        int index = letterIndex(px->imgInfo->label.at(0).toLatin1());
        ++hist.at<float>(0, index);
    }
    return hist;
}

inline float calculateEntropy(cv::Mat& hist)
{
    float entr = 0;
    float totalSize=0;
    int nCols = hist.cols;

    for(int i=0; i<nCols; ++i)
        totalSize += hist.at<float>(0, i);

    for(int i=0; i<nCols; ++i)
    {
        float nPixelsAt = hist.at<float>(0, i);
        if(nPixelsAt>0)
        {
            float probability = nPixelsAt/totalSize;
            entr -= probability*(log(probability));
        }
    }
    //cout << "ENTROPY : " << entr;
    return entr;
}

inline float calculateEntropyOfVector(PixelCloud& pixels, int labelCount)
{
    cv::Mat hist = createHistogram(pixels, labelCount);
    return calculateEntropy(hist);
}

inline void generateTeta(Coord& crd, int probe_x, int probe_y)
{
    // random number between -probe_distance, probe_distance
    crd.m_dy = (rand() % (2*probe_y)) - probe_y;
    crd.m_dx = (rand() % (2*probe_x)) - probe_x;
}

inline int generateTau()
{
    // random number between -127, +128
    return (rand() % 256) - 127;
}

inline int getTotalNumberOfPixels(const cv::Mat& hist)
{
    int totalSize =0;
    int nCols = hist.cols;
    for(int i=0; i<nCols; ++i)
        totalSize += hist.at<float>(0, i);
    return totalSize;
}

inline int getMaxLikelihoodIndex(const cv::Mat& hist)
{
    int max_val=-1;
    int max_index=0;
    for(int i=0;i<hist.cols;++i)
    {
        if(hist.at<float>(0, i) > max_val)
        {
            max_val = hist.at<float>(0, i);
            max_index = i;
        }
    }
    return max_index;
}

inline void getImageLabelVotes(const cv::Mat& label_image, cv::Mat& vote_vector)
{
    int rows = label_image.rows;
    int cols = label_image.cols;
    for (int i = 0; i < rows ; ++i)
        for (int j = 0; j < cols; ++j)
            ++vote_vector.at<float>(label_image.at<uchar>(i,j));
}



inline void printHistogram(cv::Mat &hist)
{
    QString res = "HIST {";
    for(int i=0;i<hist.cols;i++)
        res += " " + QString::number( hist.at<float>(0,i));
    qDebug() << res << "}";
}

inline cv::Mat unpad(const cv::Mat &img, int probe_x, int probe_y)
{
    int width = img.cols - 2*probe_x;
    int height = img.rows - 2*probe_y;
    cv::Mat unpadded;
    img(cv::Rect(probe_x, probe_y, width, height)).copyTo(unpadded);
    return unpadded;
}

template <typename T, typename FUNC>
void doForAllPixels(cv::Mat_<T> &M, const FUNC &func)
{
    int nRows = M.rows;
    int nCols = M.cols;
    for(int i=0; i<nRows; ++i)
    {
        auto *pRow = (T *)M.ptr(i);
        for(int j=0; j<nCols; ++j, ++pRow)
            *pRow = func(*pRow);
    }
}

class Util
{
public:
    static double calculateAccuracy(const std::vector<QString> &groundtruth, const std::vector<QString> &results);
    static cv::Mat toCv(const QImage &image, int cv_type);
    static QImage toQt(const cv::Mat &src, QImage::Format format);
    static QString cleanNumberAndPunctuation(QString toClean);
    static void plot(const cv::Mat &hist,QWidget *parent);
    static QString fileNameWithoutPath(QString& filePath);
    static void convertToOSRAndBlure(QString srcDir, QString outDir, int ksize);
    static void calcWidthHeightStat(QString srcDir);
};

#endif
