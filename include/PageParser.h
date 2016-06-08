#ifndef PAGE_PARSER
#define PAGE_PARSER

#include <QtCore>
#include <QtCore/QtCore>
#include <QtXml/QDomDocument>

#include <QDebug>
#include "QString"



class PageParser
{
  public:
    void readFromTo(QString filename, std::vector<QString> &words,
                    std::vector<QString> &coords);
    void getElements(QDomElement root, QString tagname, QString attribute1,
                     QString attribute2);
    void cropPolygons(const QString filename, QString saveDir,
                      std::vector<QString> &words,
                      std::vector<QString> &coordinates);

  private:
    std::vector<QString> *words;
    std::vector<QString> *coords;
};

#endif
