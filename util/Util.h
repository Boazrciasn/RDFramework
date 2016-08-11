#ifndef CPV_UTIL
#define CPV_UTIL

#include "ocr/HistogramDialogGui.h"
#include "rdf/PixelCloud.h"

template <typename T, typename FUNC>
void doForAllPixels(const cv::Mat_<T> &M, const FUNC &func)
{
    int nRows = M.rows;
    int nCols = M.cols;
    for(int i = 0; i < nRows; ++i)
    {
        auto *pRow = (T *)M.ptr(i);
        for(int j = 0; j < nCols; ++j, ++pRow)
            func(*pRow, i, j);
    }
}

template <typename T, typename FUNC>
void setForAllPixels(cv::Mat_<T> &M, const FUNC &func)
{
    int nRows = M.rows;
    int nCols = M.cols;
    for(int i = 0; i < nRows; ++i)
    {
        auto *pRow = (T *)M.ptr(i);
        for(int j = 0; j < nCols; ++j, ++pRow)
            *pRow = func(*pRow, i, j);
    }
}

template <typename T>
inline double sumall(const T &container)
{
    double sum{};
    for(const auto &a : container)
        sum += a;

    return sum;
}

template <typename T, typename FUNC>
inline double sumall(const T &container, const FUNC &func)
{
    double sum{};
    for(const auto &a : container)
        sum += func(a);

    return sum;
}

inline int letterIndex(char letter)
{
    return letter - 'a';
}

// creates histogram out of a pixel vector : need(?) fix after image info re-arrange.
inline cv::Mat_<float> createHistogram(PixelCloud &pixels, int labelCount)
{
    cv::Mat_<float> hist(1, labelCount);
    hist.setTo(0.0f);
    for (auto px : pixels)
    {
        int index = letterIndex(px->imgInfo->m_label.at(0).toLatin1());
        ++hist.at<float>(0, index);
    }
    return hist;
}

inline float calculateEntropy(const cv::Mat_<float> &hist)
{
    float entr{};
    float totalSize{};
    int nCols = hist.cols;
    for(int i = 0; i < nCols; ++i)
        totalSize += hist(0, i);

    for(int i = 0; i < nCols; ++i)
    {
        float nPixelsAt = hist(0, i);
        if(nPixelsAt > 0)
        {
            float probability = nPixelsAt / totalSize;
            entr -= probability * (log(probability));
        }
    }
    //cout << "ENTROPY : " << entr;
    return entr;
}

inline float calculateEntropyOfVector(PixelCloud &pixels, int labelCount)
{
    return calculateEntropy(createHistogram(pixels, labelCount));
}


inline int getTotalNumberOfPixels(const cv::Mat &hist)
{
    int totalSize = 0;
    int nCols = hist.cols;
    for(int i = 0; i < nCols; ++i)
        totalSize += hist.at<float>(0, i);
    return totalSize;
}

inline int getMaxLikelihoodIndex(const QVector<float> &hist)
{
    using namespace std;
    return distance(begin(hist), max_element(begin(hist), end(hist)));
}

inline void getImageLabelVotes(const cv::Mat_<uchar> &label_image, QVector<float> &vote_vector)
{
    doForAllPixels(label_image, [&](uchar value, int, int)
    {
        ++vote_vector[value];
    });
    //    int nRows = label_image.rows;
    //    int nCols = label_image.cols;
    //    for (int i = 0; i<nRows ; ++i)
    //        for (int j = 0; j<nCols; ++j)
    //            ++vote_vector[label_image(i,j)];
}



inline void printHistogram(cv::Mat_<float> &hist)
{
    QString res = "HIST {";
    int nRows = hist.rows;
    doForAllPixels(hist, [&](float value, int i, int j)
    {
        res += " " + QString::number(value);
        if(i != nRows - 1)
            res += "\n";
    });
    //    int nCols = hist.cols;
    //    int nRows = hist.rows;
    //    for(int i=0; i<nRows; ++i)
    //    {
    //        for(int j=0; j<nCols; ++j)
    //            res += " " + QString::number(hist.at<float>(i,j));
    //        if(i!=nRows-1)
    //            res += "\n";
    //    }
    qDebug() << res << "}";
}

inline cv::Mat unpad(const cv::Mat &img, int probe_x, int probe_y)
{
    int width = img.cols - 2 * probe_x;
    int height = img.rows - 2 * probe_y;
    cv::Mat unpadded;
    img(cv::Rect(probe_x, probe_y, width, height)).copyTo(unpadded);
    return unpadded;
}

class Util
{
  public:
    static void print3DHistogram(cv::Mat &inMat);
    static double calculateAccuracy(const std::vector<QString> &groundtruth, const std::vector<QString> &results);
    static cv::Mat toCv(const QImage &image, int cv_type);
    static void CalculateHistogram(cv::Mat &inputMat, cv::Mat &hist, int histSize);
    static QImage toQt(const cv::Mat &src, QImage::Format format);
    static QString cleanNumberAndPunctuation(QString toClean);
    static void plot(const cv::Mat &hist, QWidget *parent, const QString title);
    static QString fileNameWithoutPath(QString &filePath);
    static void convertToOSRAndBlure(QString srcDir, QString outDir, int ksize);
    static void calcWidthHeightStat(QString srcDir);
    static void averageLHofCol(cv::Mat &mat, const QVector<quint32> fgNumberCol);
    static void getWordWithConfidence(cv::Mat_<float> &mat, int nLabel, QString &word, float &conf);
    static int  countImagesInDir(QString dir);
    static void covert32FCto8UC(cv::Mat &input, cv::Mat &output);
};

#endif
