#include "include/PageParser.h"
#include <QtXml/QDomElement>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNode>
#include <QDebug>
#include <QDir>
#include <QPainter>

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

            this->words->push_back(uniCodeElement.text());
            this->coords->push_back(coordElement.attribute("points"));
        }
    }
}

void PageParser::cropPolygons(const QString filename,
                             vector<QString>& words,
                             vector<QString>& coordinates)
{
    QImage image(filename);
    for(unsigned int j=0;j<words.size();j++)
    {
        //each word represents a directory name
        QString word   =words[j];
        QString coordsStr =coordinates[j];

        //check if directory exist
        QDir dir(word);
        if (!dir.exists())
        {
            dir.mkpath(".");
        }

        QPolygon poly;
        QStringList coords = coordsStr.split(" ");
        for(int i=0; i< coords.count(); i++)
        {
            QString pointStr = coords[i];
            QStringList pointCoords = pointStr.split(",");
            int x = pointCoords[0].trimmed().toInt();
            int y = pointCoords[1].trimmed().toInt();
            poly <<  QPoint(x,y);
        }

        // image to copy polygon into
        QImage newImage(image.width(),image.height(),image.format());
        // draw only the points within the polygon
        for (int x = 0; x < image.width(); ++x)
        {
            for (int y = 0; y < image.width(); ++y)
            {
                QPoint p(x, y);
                if (poly.containsPoint(p, Qt::OddEvenFill))
                {
                    QRgb color = image.pixel(p);
                    newImage.setPixel(p, color);
                }
            }
        }

        QRect rect =  poly.boundingRect();
        QImage cropped = newImage.copy(rect);
        QFileInfo fileInfo(filename);
        QString fileNameWithoutExt = fileInfo.fileName().left(fileInfo.fileName().length()-4);
        QString fnameToSave = ".//" + word + "//" + fileNameWithoutExt + QString::number(j) + ".jpg";
        qDebug() << "fname : " << fnameToSave ;
        cropped.save(fnameToSave);

    }
}
