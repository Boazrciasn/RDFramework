#ifndef PCA_MINST_H
#define PCA_MINST_H

#include <string>
#include <vector>
#include <opencv2/core/mat.hpp>
#include "src/precompiled.h"


class MNIST
{

  public:

    MNIST(QString filepath);
    typedef std::vector<cv::Mat> &ImageDataSet;
    typedef std::vector<int> &LabelDataSet;

    void getTestImages(ImageDataSet &images);
    void getTestLabels(LabelDataSet labels);
    void getTrainImages(ImageDataSet &images);
    void getTrainLabels(LabelDataSet labels);

  private:

    QString m_testImagesFilename = "t10k-images.idx3-ubyte";
    QString m_testLabelsFilename = "t10k-labels.idx1-ubyte";
    QString m_trainLabelsFilename = "train-labels.idx1-ubyte";
    QString m_trainImagesFilename = "train-images.idx3-ubyte";
    QString m_rootPath;
    int reverseInt(int i);
    void readMINST(QString filename, ImageDataSet vec);
    void readMINSTLabel(QString filename, LabelDataSet vec);

};


#endif //PCA_MINST_H
