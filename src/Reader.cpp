#include "precompiled.h"

#include "include/Reader.h"

void Reader::readFromTo(std::string filename, std::vector<QString> &imgName)
{
    struct dirent *ent;

    // check for valid directory
    if ((m_dir = opendir(filename.c_str())) != NULL)
    {
        QString prev = "abdd";

        // read from folder until it is empty
        while((ent = readdir(m_dir)) != NULL)
        {
            // get filename and convert it to string
            std::string str = (std::string)ent->d_name;
            size_t pos = str.find(".");

            if (pos == std::string::npos || pos == 0)
                continue;

            //            cout<<str.substr(0,pos)<<endl;
            QString file = QString::fromStdString(str.substr(0, pos));

            if(prev != file)
            {
                imgName.push_back(file);
                prev = file;
            }
        }

        closedir(m_dir);
    }

    else
        std::cerr << "Couldn't open directory!" << std::endl;
}


void Reader::findImages(QString baseDir, QString query,
                        std::vector<QString> &foundImages , std::vector<QString> &labels)
{
    query = "*" + query + "*";
    QDirIterator it(baseDir, QStringList() << query,
                    QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive) ;

    while (it.hasNext())
    {
        it.next();
        QDirIterator itFile(baseDir + "/" + it.fileName(),
                            QStringList() << "*.jpg" << "*.jpeg" << "*.png", QDir::Files);

        while(itFile.hasNext())
        {
            itFile.next();
            foundImages.push_back(itFile.filePath());
            labels.push_back(it.fileName());
        }
    }
}

void Reader::findImages(QString baseDir, QString query,
                        std::vector<QString> &foundImages)
{
    query = "*" + query + "*";
    QDirIterator it(baseDir, QStringList() << query,
                    QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive) ;

    while (it.hasNext())
    {
        it.next();
        QDirIterator itFile(baseDir + "/" + it.fileName(),
                            QStringList() << "*.jpg" << "*.jpeg" << "*.png", QDir::Files);

        while(itFile.hasNext())
        {
            itFile.next();
            foundImages.push_back(itFile.filePath());
        }
    }
}

void Reader::readTextFiles(QString baseDir, QString query,
                           std::vector<QString> &foundText)
{
    QDirIterator it(baseDir, QStringList() << query,
                    QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive) ;

    while (it.hasNext())
    {
        it.next();
        QDirIterator itFile(baseDir + "/" + it.fileName(), QStringList() << "*.txt",
                            QDir::Files);

        while(itFile.hasNext())
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

        while(itFile.hasNext())
        {
            itFile.next();
            foundText.push_back(itFile.filePath());
        }
    }
}
