#include "include/PageParser.h"
#include <QtXml/QDomElement>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNode>
#include <QDebug>

void PageParser::readFromTo(QString filename, vector<QString> &words, vector<QString> &coords)
{
    this->words = &words;
    this->coords = &coords;

    QDomDocument document;

    //Load the file
    QFile file(filename);
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

            qDebug() << uniCodeElement.text();
            qDebug() << itemele.attribute(attribute1);
            this->words->push_back(uniCodeElement.text());
            this->coords->push_back(itemele.attribute(attribute1));
        }
    }
}
