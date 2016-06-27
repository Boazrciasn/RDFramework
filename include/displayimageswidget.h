#ifndef DISPLAYIMAGESWIDGET_H
#define DISPLAYIMAGESWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QFileDialog>
#include <QLabel>
#include <QImage>
#include <QThread>

#include "include/PageParser.h"
#include "include/Reader.h"
#include "include/mydialog.h"
#include "histogramdialog.h"
#include "include/PageParser.h"
#include "include/Util.h"

namespace Ui
{
class DisplayImagesWidget;
}

class DisplayImagesWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit DisplayImagesWidget(QWidget *parent = 0);
    ~DisplayImagesWidget();

  public slots:
    void display();

  private slots:
    void extractWords();
    void browseButton_clicked();
    void prevButton_clicked();
    void nextButton_clicked();

  private:
    Ui::DisplayImagesWidget *ui;
    PageParser *m_pageParser;
    Reader *m_reader;
    int m_fileIndex;
    std::vector<QString> m_fNames;
    QString m_dir;
    std::vector<QString> m_words;     // file names are stored
    std::vector<QString> m_coords;     // file names are stored
};

#endif // DISPLAYIMAGESWIDGET_H
