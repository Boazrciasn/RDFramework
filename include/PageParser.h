#ifndef PAGE_PARSER
#define PAGE_PARSER

#include <QtCore>
#include <QtCore/QtCore>
#include <QtXml/QDomDocument>

#include <QDebug>
#include "QString"

using namespace std;

class PageParser{
public:
    void readFromTo(QString filename, vector<QString> &words, vector<QString> &coords);
    void getElements(QDomElement root, QString tagname, QString attribute1, QString attribute2);
private:
    vector<QString> *words;
    vector<QString> *coords;
};

#endif
