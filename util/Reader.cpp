#include "precompiled.h"

#include "Reader.h"

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

void Reader::findImages(QString baseDir, std::vector<QString> &foundImages)
{
    QDirIterator itFile(baseDir, QStringList() << "*.jpg" << "*.jpeg" << "*.png", QDir::Files,
                        QDirIterator::Subdirectories);
    while (itFile.hasNext())
    {
        itFile.next();
        foundImages.push_back(itFile.filePath());
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
