#include <iostream>
#include <fstream>

#include "MNIST.h"


// TODO: Paths must be input from UI


MNIST::MNIST(QString filepath) : m_rootPath(filepath) {}


int MNIST::reverseInt(int i)
{
    unsigned char ch1, ch2, ch3, ch4;
    ch1 = (unsigned char) (i & 255);
    ch2 = (unsigned char) ((i >> 8) & 255);
    ch3 = (unsigned char) ((i >> 16) & 255);
    ch4 = (unsigned char) ((i >> 24) & 255);
    return((int) ch1 << 24) + ((int)ch2 << 16) + ((int)ch3 << 8) + ch4;
}

void MNIST::getTestImages(ImageDataSet &images)
{
    MNIST::readMINST(MNIST::m_testImagesFilename, images);
}

void MNIST::getTestLabels(LabelDataSet labels)
{
    MNIST::readMINSTLabel(MNIST::m_testLabelsFilename, labels);
}


void MNIST::getTrainImages(ImageDataSet &images)
{
    MNIST::readMINST(MNIST::m_trainImagesFilename, images);
}

void MNIST::getTrainLabels(LabelDataSet labels)
{
    MNIST::readMINSTLabel(MNIST::m_trainLabelsFilename, labels);
}

void MNIST::readMINST(QString filename, ImageDataSet vec)
{
    QString path = m_rootPath + filename;
    std::ifstream file(path.toStdString(), std::ios::binary);

    if (file.is_open())
    {
        int magic_number = 0;
        int number_of_images = 0;
        int n_rows = 0;
        int n_cols = 0;
        file.read((char *) &magic_number, sizeof(magic_number));
        magic_number = MNIST::reverseInt(magic_number);
        file.read((char *) &number_of_images, sizeof(number_of_images));
        number_of_images = MNIST::reverseInt(number_of_images);
        file.read((char *) &n_rows, sizeof(n_rows));
        n_rows = MNIST::reverseInt(n_rows);
        file.read((char *) &n_cols, sizeof(n_cols));
        n_cols = MNIST::reverseInt(n_cols);

        for (int i = 0; i < number_of_images; ++i)
        {
            cv::Mat tp = cv::Mat::zeros(n_rows, n_cols, CV_32FC1);

            for (int r = 0; r < n_rows; ++r)
            {
                for (int c = 0; c < n_cols; ++c)
                {
                    unsigned char temp = 0;
                    file.read((char *) &temp, sizeof(temp));
                    tp.at<float>(r, c) = (float) (1.0 - ((float) temp) / 255.0);
                }
            }

            vec.push_back(tp);
        }
    }

    else
    {
        qDebug() << filename << " not found !";
    }
}

void MNIST::readMINSTLabel(QString filename, LabelDataSet vec)
{
    //TODO: Refactor due to our needs :
    QString path = m_rootPath + filename;
    std::ifstream file(path.toStdString(), std::ios::binary);

    if (file.is_open())
    {
        int magic_number = 0;
        int number_of_images = 0;
        file.read((char *) &magic_number, sizeof(magic_number));
        magic_number = MNIST::reverseInt(magic_number);
        file.read((char *) &number_of_images, sizeof(number_of_images));
        number_of_images = MNIST::reverseInt(number_of_images);

        for (int i = 0; i < number_of_images; ++i)
        {
            unsigned char temp = 0;
            file.read((char *) &temp, sizeof(temp));
            vec.push_back((int) temp);
        }
    }

    else
    {
        qDebug() << filename << " not found !";
    }
}


