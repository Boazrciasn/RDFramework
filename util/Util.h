#ifndef CPV_UTIL
#define CPV_UTIL

#include "ocr/HistogramDialogGui.h"
#include "rdf/PixelCloud.h"

using Width = quint16;
using Height = quint16;

class Animal
{
  public:
    Animal() {}
    virtual void execute() { qDebug() <<  "No cats or dogs" ;}
};


template<class T>
class Cat : public Animal
{
  public :
    T m_param;
    Cat(T param) { m_param = param;}
    void execute() override { qDebug() << m_param << " Cats" ;}
};

template<class T>
class Dog : public Animal
{
  public :
    T m_param;
    Dog(T param)
    {
        m_param = param;
    }
    void execute() { qDebug() << m_param << " Cats";}
};





//Curiously recurring template pattern :
template <typename DERIVED>
class Base
{
  public :
    Base() {}
    void process()
    {
        static_cast<DERIVED *>(this)->run();
    }
  protected:
    void trythisout()
    {
        std::cout << "check this out " << std::endl;
    }
};

class GaussianProcess : public Base<GaussianProcess>
{
  public :
    GaussianProcess(int a, int b) : m_a(a), m_b(b) {}
    int m_a;
    int m_b;
    void run()
    {
        trythisout();
        std::cout << " Gaussian Hello!?!? " << m_a << m_b << std::endl;
    }
};

class LaplacianProcess : public Base<GaussianProcess>
{
  public:
    void run()
    {
        std::cout << "Laplacian Hello!?!?" << std::endl;
    }
};

template<typename DERIVED>
class NodeTest
{
  public:
    NodeTest() {}
    void doprocess()
    {
        static_cast<DERIVED *>(this)->runFeature();
    }
};

class HAARNode : public NodeTest<HAARNode>
{
  public :
    void runFeature()
    {
        std::cout << "Selecting best HAAR" << std::endl;
    }
};




template <typename T, typename FUNC>
void doForAllPixels(const cv::Mat_<T> &M, const FUNC &func)
{
    int nRows = M.rows;
    int nCols = M.cols;
    for (quint16 i = 0; i < nRows; ++i)
    {
        auto *pRow = (T *)M.ptr(i);
        for (quint16 j = 0; j < nCols; ++j, ++pRow)
            func(*pRow, i, j);
    }
}


template <typename T, typename FUNC>
void setForAllPixels(cv::Mat_<T> &M, const FUNC &func)
{
    Height nRows = M.rows;
    Width nCols = M.cols;
    for (Height i = 0; i < nRows; ++i)
    {
        auto *pRow = (T *)M.ptr(i);
        for (Width j = 0; j < nCols; ++j, ++pRow)
            *pRow = func(*pRow, i, j);
    }
}

template <typename T>
inline double sumall(const T &container)
{
    double sum{};
    for (const auto &a : container)
        sum += a;
    return sum;
}

template <typename T, typename FUNC>
inline double sumall(const T &container, const FUNC &func)
{
    double sum{};
    for (const auto &a : container)
        sum += func(a);
    return sum;
}

struct TableLookUp
{
    static constexpr int size = 1 << 18;
    static tableLookupType lookUp[size];
    TableLookUp() {}

    static void init();
};

// creates histogram out of a pixel vector : need(?) fix after image info re-arrange.
inline cv::Mat_<float> createHistogram(PixelCloud &pixels, int labelCount)
{
    cv::Mat_<float> hist(1, labelCount);
    hist.setTo(0.0f);
    for (auto px : pixels.pixels1)
        ++hist.at<float>(0, px.label);
    return hist;
}

inline float calculateEntropyLookUp(const cv::Mat_<quint32>& hist)
{
    auto entr = 0.0f;
    auto totalPx = 0;
    auto nCols = hist.cols;

    for (int i = 0; i < nCols; ++i)
    {
        entr    += TableLookUp::lookUp[hist(0,i)];
        totalPx += hist(0,i);
    }

    return TableLookUp::lookUp[totalPx] - entr;
}

inline float calculateEntropy(const cv::Mat_<quint32>& hist)
{
    auto entr = 0.0f;
    auto totalPx = 0;
    auto nCols = hist.cols;

    for (int i = 0; i < nCols; ++i)
    {
        auto nPixelsAt = hist(0,i);
        totalPx += nPixelsAt;
        if (nPixelsAt > 0)
            entr += nPixelsAt*log(nPixelsAt);
    }

    return totalPx*log(totalPx) - entr;
}

inline float calculateEntropyProb(const cv::Mat_<float> &hist)
{
    float entr{};
    float totalNPixels = cv::sum(hist)[0];
    int nCols = hist.cols;
    for (int i = 0; i < nCols; ++i)
    {
        float nPixelsAt = hist(0, i);
        if (nPixelsAt > 0)
        {
            float probability = nPixelsAt / totalNPixels;
            entr -= probability * (log(probability));
        }
    }
    return entr;
}

inline float calculateEntropyOfVector(PixelCloud &pixels, int labelCount)
{
    return calculateEntropyLookUp(createHistogram(pixels, labelCount));
}

inline int getMaxLikelihoodIndex(const QVector<float> &hist)
{
    using namespace std;
    return distance(begin(hist), max_element(begin(hist), end(hist)));
}

inline void getImageLabelVotes(const cv::Mat_<uchar> &label_image, QVector<float> &vote_vector)
{
    doForAllPixels(label_image, [&](uchar value, quint16, quint16)
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
    doForAllPixels(hist, [&](float value, quint16 i, quint16 j)
    {
        res += " " + QString::number(value);
        if (i != nRows - 1)
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
  private:
    static inline std::vector<int> regionQuery(std::vector<cv::Point> *points, cv::Point *point, float eps)
    {
        using namespace std;
        float dist;
        int ptTotal = points->size();
        vector<int> retKeys;
        for (auto i = 0; i < ptTotal; ++i)
        {
            dist = sqrt(pow((point->x - points->at(i).x), 2) + pow((point->y - points->at(i).y), 2));
            if (dist <= eps && dist != 0.0f)
                retKeys.push_back(i);
        }
        return retKeys;
    }
  public:
    static void print3DHistogram(cv::Mat &inMat);
    static double calculateAccuracy(const std::vector<QString> &groundtruth, const std::vector<QString> &results);
    static cv::Mat toCv(const QImage &image, int cv_type);
    static void CalculateHistogram(cv::Mat &inputMat, cv::Mat &hist, int histSize);
    static QImage toQt(const cv::Mat &src, QImage::Format format);
    static QImage Mat2QImage(const cv::Mat3b &src);
    static QString cleanNumberAndPunctuation(QString toClean);
    static void plot(const cv::Mat &hist, QWidget *parent, const QString title);
    static QString fileNameWithoutPath(QString &filePath);
    static void convertToOSRAndBlur(QString srcDir, QString outDir, int ksize);
    static void calcWidthHeightStat(QString srcDir);
    static void averageLHofCol(cv::Mat &mat, const QVector<quint32> fgNumberCol);
    static void getWordWithConfidence(cv::Mat_<float> &mat, int nLabel, QString &word, float &conf);
    static int  countImagesInDir(QString dir);
    static void covert32FCto8UC(cv::Mat &input, cv::Mat &output);
    static std::vector<cv::Rect> calculateBoundingBoxRect(const cv::Mat_<quint8> &inputImg, quint16 minSize ,
                                                          quint16 maxSize, double aspectMax, double aspectMin);
    static void inline drawBoundingBox(cv::Mat &inputImg, const std::vector<cv::Rect> &boundingBoxes)
    {
        cv::RNG rng(12345);
        for (size_t i = 0; i < boundingBoxes.size() ; ++i)
        {
            cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
            rectangle(inputImg, boundingBoxes[i].tl(), boundingBoxes[i].br(), color, 2, 8, 0);
        }
    }
    static void inline Erosion(const cv::Mat &inputImg, cv::Mat outputImg, quint16 size, cv::MorphShapes elementType)
    {
        cv::Mat element = cv::getStructuringElement(elementType, cv::Size(2 * size + 1, 2 * size + 1), cv::Point(size, size));
        cv::erode(inputImg, outputImg, element);
    }
    static void inline Dilation(const cv::Mat &inputImg, cv::Mat outputImg, quint16 size, cv::MorphShapes elementType)
    {
        cv::Mat element = cv::getStructuringElement(elementType, cv::Size(2 * size + 1, 2 * size + 1), cv::Point(size, size));
        cv::dilate(inputImg, outputImg, element);
    }
    static void writeMatToFile(cv::Mat &m, const char *filename);
    static std::vector<std::vector<cv::Point> > DBSCAN_points(std::vector<cv::Point> *points, float eps,
                                                              unsigned int minPts);
};

#endif
