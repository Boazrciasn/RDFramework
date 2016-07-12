#ifndef PCA_MINST_H
#define PCA_MINST_H
#include "src/precompiled.h"

#include <string>
#include <vector>
#include <opencv2/core/mat.hpp>

#include "Util.h"



class MNIST
{

    using vMatSize = std::vector<cv::Mat>::size_type;
  public:

    typedef std::vector<cv::Mat> *ImageDataSet;
    typedef std::vector<int> *LabelDataSet;
    std::vector<int> m_labels = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    MNIST(QString filepath);
    void MNISTReader();
    ImageDataSet m_trainImagesVector;
    ImageDataSet m_testImagesVector;
    LabelDataSet m_trainLabels;
    LabelDataSet m_testLabels;
    ImageDataSet getTestImages();
    LabelDataSet getTestLabels();
    ImageDataSet getTrainImages();
    LabelDataSet getTrainLabels();
    inline void saveImage(QString &savedir, QImage img)
    {
        if(!img.save(savedir))
        {
            qDebug() << "Error Saving File !";
            return;
        }
    }
    void extractDataSet(QString destdir);
    void saveDataSet(QString &destdir, ImageDataSet imageDataSet, LabelDataSet imageLabels);

  private:

    void createSaveDirs(QString destdir, QString &trainpath, QString &testpath);
    QString m_testImagesFilename = "t10k-images.idx3-ubyte";
    QString m_testLabelsFilename = "t10k-labels.idx1-ubyte";
    QString m_trainLabelsFilename = "train-labels.idx1-ubyte";
    QString m_trainImagesFilename = "train-images.idx3-ubyte";
    QString m_rootPath;
    int reverseInt(int i);
    void readMINST(QString filename, ImageDataSet vec);
    void readMINSTLabel(QString filename, LabelDataSet vec);
    void writeHeader(QFile *logfile, int sampleCount, QString label);
    void writeEntry(QFile *logfile, QString imgName);
    void createSaveFiles(QVector<QFile *> &logfiles, QString path);
    void writeHeaders(QVector<QFile *> &logfiles, const QVector<int> &vec_samplecounts);
};


#endif //PCA_MINST_H
