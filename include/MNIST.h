#ifndef PCA_MINST_H
#define PCA_MINST_H

#include <string>
#include <vector>
#include <opencv2/core/mat.hpp>


class MNIST
{

  public:

    typedef std::vector<cv::Mat> *ImageDataSet;
    typedef std::vector<int> *LabelDataSet;

    static ImageDataSet getTestImages();
    static LabelDataSet getTestLabels();
    static ImageDataSet getTrainImages();
    static LabelDataSet getTrainLabels();
    // TODO: Paths must be input from UI
    static std::string rootPath;
    static std::string testImagesFilename;
    static std::string testLabelsFilename;
    static std::string trainLabelsFilename;
    static std::string trainImagesFilename;

  private:

    static int reverseInt(int i);
    static void readMINST(std::string filename, ImageDataSet vec);
    static void readMINSTLabel(std::string filename, LabelDataSet vec);

};


#endif //PCA_MINST_H
