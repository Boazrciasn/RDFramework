#ifndef PCA_MINST_H
#define PCA_MINST_H
#include "precompiled.h"

#include <string>
#include <vector>
#include <opencv2/core/mat.hpp>

#include "Util.h"



class MNIST
{

  public:

    typedef std::vector<cv::Mat> ImageDataSet;
    typedef std::vector<int> LabelDataSet;
    std::vector<int> m_labels = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ImageDataSet m_trainImagesVector;
    ImageDataSet m_testImagesVector;
    LabelDataSet m_trainLabels;
    LabelDataSet m_testLabels;
    bool readMINST(QString path, ImageDataSet &vec);
    bool readMINSTLabel(QString path, LabelDataSet &vec);

  private:

    QString m_testImagesFilename = "t10k-images.idx3-ubyte";
    QString m_testLabelsFilename = "t10k-labels.idx1-ubyte";
    QString m_trainLabelsFilename = "train-labels.idx1-ubyte";
    QString m_trainImagesFilename = "train-images.idx3-ubyte";
    int reverseInt(int i);
};


#endif //PCA_MINST_H
