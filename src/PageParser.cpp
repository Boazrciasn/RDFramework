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

//    // TEST CROP IMAGE
//    //QString fname = "/home/mahiratmis/Desktop/images2.jpg";
//    std::vector<QString> wordss;
//    std::vector<QString> coordss;

//    wordss.push_back("16");
//    coordss.push_back("357,157 430,157 430,207 357,207");

//    wordss.push_back("Aug");
//    coordss.push_back("454,163 572,163 572,238 454,238");

//    wordss.push_back("Aug");
//    coordss.push_back("276,410 470,370 1093,383 1853,390 1926,426 1910,560 1930,673 1976,1210 1950,1516 1966,2426 923,2466 786,2543 250,2543 253,1456 263,633 266,463");

}

void PageParser::getElements(QDomElement root, QString tagname, QString attribute1, QString attribute2)
{
    QDomNodeList items = root.elementsByTagName(tagname);
    qDebug() << "Total items = " << items.count();

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


            qDebug() << uniCodeElement.text();
            qDebug() << coordElement.attribute("points");
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
