#ifndef PCA_MINST_H
#define PCA_MINST_H

#include <string>
#include <vector>
#include <opencv2/core/mat.hpp>


class MNIST
{

  public:

    MNIST(std::string filepath);
    typedef std::vector<cv::Mat> &ImageDataSet;
    typedef std::vector<int> &LabelDataSet;

    void getTestImages(ImageDataSet &images);
    void getTestLabels(LabelDataSet labels);
    void getTrainImages(ImageDataSet &images);
    void getTrainLabels(LabelDataSet labels);

  private:

    std::string m_testImagesFilename = "t10k-images.idx3-ubyte";
    std::string m_testLabelsFilename = "t10k-labels.idx1-ubyte";
    std::string m_trainLabelsFilename = "train-labels.idx1-ubyte";
    std::string m_trainImagesFilename = "train-images.idx3-ubyte";
    std::string m_rootPath;
    int reverseInt(int i);
    void readMINST(std::string filename, ImageDataSet vec);
    void readMINSTLabel(std::string filename, LabelDataSet vec);

};


#endif //PCA_MINST_H
