

#include "Reader.h"

#include "ReaderMNIST.h"


void Reader::findImages(QString baseDir, QString query, std::vector<QString> &foundImages ,
                        std::vector<QString> &labels)
{
    query = "*" + query + "*";
    QDirIterator it(baseDir, QStringList() << query, QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive) ;
    while (it.hasNext())
    {
        it.next();
        QDirIterator itFile(baseDir + "/" + it.fileName(), QStringList() << "*.jpg" << "*.jpeg" << "*.png", QDir::Files);
        while (itFile.hasNext())
        {
            itFile.next();
            foundImages.push_back(itFile.filePath());
            labels.push_back(it.fileName());
        }
    }
}

void Reader::findImages(QString baseDir, QString query, std::vector<QString> &foundImages)
{
    query = "*" + query + "*";
    QDirIterator it(baseDir, QStringList() << query,
                    QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive) ;
    while (it.hasNext())
    {
        it.next();
        QDirIterator itFile(baseDir + "/" + it.fileName(),
                            QStringList() << "*.jpg" << "*.jpeg" << "*.png", QDir::Files);
        while (itFile.hasNext())
        {
            itFile.next();
            foundImages.push_back(itFile.filePath());
        }
    }
}

void Reader::readImages(QString dir, std::vector<cv::Mat> &images, int flags = Type_Standard)
{
    switch (flags)
    {
    case Type_Standard:
    {
        QDirIterator itFile(dir, QStringList() << "*.jpg" << "*.jpeg" << "*.png", QDir::Files,
                            QDirIterator::Subdirectories);
        while (itFile.hasNext())
        {
            itFile.next();
            cv::Mat image = cv::imread(itFile.filePath().toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
            images.push_back(image);
        }
        break;
    }
    case Type_MNIST:
    {
        MNIST mnist;
        mnist.readMINST(dir, images);
        break;
    }
    default:
        break;
    }
}

void Reader::readImages(QString dir, std::vector<QString> &foundImages)
{
    QDirIterator itFile(dir, QStringList() << "*.jpg" << "*.jpeg" << "*.png", QDir::Files,
                        QDirIterator::Subdirectories);
    while (itFile.hasNext())
    {
        itFile.next();
        foundImages.push_back(itFile.filePath());
    }
}

void Reader::readLabels(QString dir, std::vector<int> &labels, int flags)
{
    switch (flags)
    {
    case Type_Standard:
    {
        // TO DO : Create a custom label storage type and its reader!
        break;
    }
    case Type_MNIST:
    {
        MNIST mnist;
        mnist.readMINSTLabel(dir, labels);
        break;
    }
    default:
        break;
    }
}

void Reader::readTextFiles(QString baseDir, QString query, std::vector<QString> &foundText)
{
    QDirIterator it(baseDir, QStringList() << query,
                    QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive) ;
    while (it.hasNext())
    {
        it.next();
        QDirIterator itFile(baseDir + "/" + it.fileName(), QStringList() << "*.txt",
                            QDir::Files);
        while (itFile.hasNext())
        {
            itFile.next();
            foundText.push_back(itFile.filePath());
        }
    }
}


void Reader::readTextFiles(QString baseDir, std::vector<QString> &foundText)
{
    QDirIterator it(baseDir, QStringList() << "*",
                    QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive) ;
    while (it.hasNext())
    {
        it.next();
        QDirIterator itFile(baseDir + "/" + it.fileName(), QStringList() << "*.txt",
                            QDir::Files);
        while (itFile.hasNext())
        {
            itFile.next();
            foundText.push_back(itFile.filePath());
        }
    }
}
