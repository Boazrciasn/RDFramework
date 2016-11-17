#include <iostream>
#include <fstream>
#include <vector>

#include "ReaderMNIST.h"


int MNIST::reverseInt(int i)
{
    unsigned char ch1, ch2, ch3, ch4;
    ch1 = (unsigned char)(i & 255);
    ch2 = (unsigned char)((i >> 8) & 255);
    ch3 = (unsigned char)((i >> 16) & 255);
    ch4 = (unsigned char)((i >> 24) & 255);
    return ((int) ch1 << 24) + ((int)ch2 << 16) + ((int)ch3 << 8) + ch4;
}


bool MNIST::readMINST(QString path, ImageDataSet &vec)
{
    QFile f(path);
    QFileInfo fileInfo(f.fileName());
    QString filename(fileInfo.fileName());
    std::ifstream file(path.toStdString(), std::ios::binary);
    if (filename != m_testImagesFilename && filename != m_trainImagesFilename)
    {
        QMessageBox messageBox(QMessageBox::Warning, "Error", "This is not a MNIST compatible File !");
        messageBox.exec();
        return false;
    }
    else if (!file.is_open())
    {
        QMessageBox messageBox(QMessageBox::Warning, "Error", "File can not be opened!");
        messageBox.exec();
        return false;
    }
    else
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
                    tp.at<float>(r, c) = (float)(1.0 - ((float) temp) / 255.0);
                }
            }
            tp.convertTo(tp,CV_8UC1,255);
            vec.push_back(tp);
        }
        file.close();
        return true;
    }
}

bool MNIST::readMINSTLabel(QString path, LabelDataSet &vec)
{
    QFile f(path);
    QFileInfo fileInfo(f.fileName());
    QString filename(fileInfo.fileName());
    std::ifstream file(path.toStdString(), std::ios::binary);
    if (filename != m_testLabelsFilename && filename != m_trainLabelsFilename)
    {
        QMessageBox messageBox(QMessageBox::Warning, "Error", "This is not a MNIST compatible File !");
        messageBox.exec();
        return false;
    }
    else if (!file.is_open())
    {
        QMessageBox messageBox(QMessageBox::Warning, "Error", "File can not be opened !");
        messageBox.exec();
        return false;
    }
    else
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
        file.close();
        return true;

    }

}
