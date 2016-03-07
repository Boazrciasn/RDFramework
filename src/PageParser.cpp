#include "include/PageParser.h"
#include "include/Util.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QtXml/QDomElement>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNode>
#include <QDebug>
#include <QDir>
#include <QPainter>
#include <QChar>

void PageParser::readFromTo(QString filename, vector<QString> &words, vector<QString> &coords)
{
    this->words = &words;
    this->coords = &coords;

    QDomDocument document;

    //Load the file
    QFile file(filename+ ".xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file";
    }
    else
    {
        if(!document.setContent(&file))
        {
            qDebug() << "Failed to load document";
        }
        file.close();
    }

    //get the root element
    QDomElement root = document.firstChildElement();
    getElements(root, "Word", "Coords","Unicode");

}

void PageParser::getElements(QDomElement root, QString tagname, QString attribute1, QString attribute2)
{
    QDomNodeList items = root.elementsByTagName(tagname);

    for(int i = 0; i < items.count(); i++)
    {
        QDomNode itemnode = items.at(i);

        //convert to element
        if(itemnode.isElement())
        {
            QDomElement itemele = itemnode.toElement();

            QDomNodeList uniCodes = itemele.elementsByTagName(attribute2);
            QDomNode uniCodeNode = uniCodes.at(0);
            QDomElement uniCodeElement = uniCodeNode.toElement();

            QDomNodeList coords = itemele.elementsByTagName(attribute1);
            QDomNode coordNode = coords.at(0);
            QDomElement coordElement = coordNode.toElement();

            QString word = uniCodeElement.text();
//            word = Util::cleanNumberAndPunctuation(word);
            if(word != "")
            {
                this->words->push_back(word);
                this->coords->push_back(coordElement.attribute("points"));
            }
        }
    }
}

void PageParser::cropPolygons(const QString filename, QString saveDir,
                             vector<QString>& words,
                             vector<QString>& coordinates)
{

    QImage image(filename + ".jpg");
    qDebug() << "Scanning " << (filename + ".jpg");
    qDebug() << "Number of words :" << words.size();
    qDebug() << "Number of words :" << coordinates.size();

    for(unsigned int j=0;j<words.size();j++)
    {
        //each word represents a directory name
        QString word   = saveDir + "/" + words[j];
        //check if directory exist
        QDir dir(word);
        if (!dir.exists())
        {
            dir.mkpath(".");
            if(!dir.exists())
                 qDebug() << "ERROR : " << dir << " can not be created!" ;
        }

        QString coordsStr =coordinates[j];
        QStringList coords = coordsStr.split(" ");
        QPolygon poly;
        for(int i=0; i< coords.count(); i++)
        {
            QString pointStr = coords[i];
            QStringList pointCoords = pointStr.split(",");
            int x = pointCoords[0].trimmed().toInt();
            int y = pointCoords[1].trimmed().toInt();
            poly <<  QPoint(x,y);
        }

        QRect rect =  poly.boundingRect();
        QImage cropped = image.copy(rect);
        cv::Mat croppedMat = Util::toCv(cropped,CV_8UC4);
        cv::Mat im_gray, img_bw;
        cv::cvtColor(croppedMat, im_gray, CV_BGR2GRAY);
        cv::threshold(im_gray, img_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
//        QImage saveQIM = Util::toQt(img_bw, QImage::Format_RGB888);
        QImage saveQIM = Util::toQt(im_gray, QImage::Format_RGB888);

        QFileInfo fileInfo(filename);
        QString fileNameWithoutExt = fileInfo.fileName();
        QString fnameToSave = word + "/" + fileNameWithoutExt + QString::number(j) + ".jpg";
        if(!saveQIM.save(fnameToSave))
             qDebug() << "ERROR : " << fnameToSave << " can not be saved!" ;

    }
}
