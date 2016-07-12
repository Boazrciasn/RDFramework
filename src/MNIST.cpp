#include <iostream>
#include <fstream>
#include <vector>

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

MNIST::ImageDataSet MNIST::getTestImages()
{
    ImageDataSet vec = new std::vector<cv::Mat>();
    readMINST(m_testImagesFilename, vec);
    return vec;
}

MNIST::LabelDataSet MNIST::getTestLabels()
{
    LabelDataSet vec = new std::vector<int>();
    readMINSTLabel(m_testLabelsFilename, vec);
    return vec;
}


MNIST::ImageDataSet MNIST::getTrainImages()
{
    ImageDataSet vec = new std::vector<cv::Mat>();
    readMINST(m_trainImagesFilename, vec);
    return vec;
}

MNIST::LabelDataSet MNIST::getTrainLabels()
{
    LabelDataSet vec = new std::vector<int>();
    readMINSTLabel(m_trainLabelsFilename, vec);
    return vec;
}

void MNIST::extractDataSet(QString destdir)
{
    QString testpath;
    QString trainpath;
    createSaveDirs(destdir, trainpath, testpath);
    saveDataSet(trainpath, m_trainImagesVector, m_trainLabels);
    saveDataSet(testpath, m_testImagesVector, m_testLabels);
    qDebug() << "Dataset extraction done ! ";
}

void MNIST::writeHeader(QFile *logfile, int sampleCount, QString label)
{
    logfile->open(QIODevice::ReadWrite | QIODevice::Text);
    QString s;
    QTextStream t(logfile);
    s += QString::number(16) + "\n";
    s += QString::number(16) + "\n";
    s += QString::number(sampleCount) + "\n";
    s += QString::number(0) + "\n";
    s += label + "\n";
    int ctr = 0;
    while(!t.atEnd())
    {
        QString line = t.readLine();
        if(++ctr > 5)
            s.append(line + "\n");
    }
    logfile->resize(0);
    t << s;
    logfile->flush();
    logfile->close();
}

void MNIST::writeHeaders(QVector<QFile *> &logfiles, const QVector<int> &vec_samplecounts)
{
    for (quint8 i = 0 ; i < logfiles.size(); ++i)
    {
        writeHeader(logfiles[i], vec_samplecounts[i], QString::number(i));
    }
}

void MNIST::writeEntry(QFile *logfile, QString imgName)
{
    logfile->open(QIODevice::Append);
    logfile->write(imgName.toUtf8());
    logfile->write(" 16 16 0 0 \n");
    logfile->flush();
    logfile->close();
}

void MNIST::createSaveFiles(QVector<QFile *> &logfiles, QString path)
{
    for (quint8 i = 0 ; i < logfiles.size(); ++i)
    {
        logfiles[i] = new QFile(path + "/" + QString::number(i) + "_lastSession.txt");
    }
}


void MNIST::saveDataSet(QString &destdir, ImageDataSet imageDataSet, LabelDataSet imageLabels)
{
    QVector<QFile *> logfiles(10);
    QVector<int> vec_samplecounts(10, 0);
    QString logDir = destdir + "/LogFiles/";
    createSaveFiles(logfiles, logDir);
    std::cout << imageDataSet->size() << std::endl;
    for(vMatSize i = 0 ; i < imageDataSet->size(); ++i)
    {
        QString label = QString::number(imageLabels->at(i));
        vec_samplecounts[imageLabels->at(i)]++;
        QString savedir = destdir + "/";
        int img_count = Util::countImagesInDir(savedir);
        QString imageName = label + "_" + QString::number(img_count) + ".jpg";
        savedir += imageName;
        cv::Mat  temp_img ;
        Util::covert32FCto8UC(imageDataSet->at(i), temp_img);
        QImage img = Util::toQt(temp_img, QImage::Format_RGB888);
        saveImage(savedir, img);
        writeEntry(logfiles[imageLabels->at(i)], imageName);
    }
    writeHeaders(logfiles, vec_samplecounts);
}


void MNIST::createSaveDirs(QString destdir, QString &trainpath, QString &testpath)
{
    QDir dir(destdir);
    dir.mkpath("MNIST_DataSet/Test");
    dir.mkpath("MNIST_DataSet/Train");
    dir.mkpath("MNIST_DataSet/Test/LogFiles");
    dir.mkpath("MNIST_DataSet/Train/LogFiles");
    testpath = destdir + "MNIST_DataSet/Test/";
    trainpath = destdir + "MNIST_DataSet/Train/";
}



void MNIST::MNISTReader()
{
    m_testImagesVector = getTestImages();
    m_trainImagesVector = getTrainImages();
    m_testLabels = getTestLabels();
    m_trainLabels = getTrainLabels();
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
            vec->push_back(tp);
        }
    }
    else
    {
        qDebug() << filename << " not found !";
    }
}

void MNIST::readMINSTLabel(QString filename, LabelDataSet vec)
{
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
            vec->push_back((int) temp);
        }
    }
    else
    {
        qDebug() << filename << " not found !";
    }
}



