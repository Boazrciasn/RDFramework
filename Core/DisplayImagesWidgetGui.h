#ifndef DISPLAYIMAGESWIDGET_H
#define DISPLAYIMAGESWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QFileDialog>
#include <QLabel>
#include <QImage>
#include <QThread>

#include "ocr/PageParser.h"
#include "ocr/Reader.h"
#include "ocr/MyDialogGui.h"
#include "ocr/HistogramDialogGui.h"
#include "Util.h"

namespace Ui
{
class DisplayImagesWidgetGui;
}

class DisplayImagesWidgetGui : public QWidget
{
    Q_OBJECT

  public:
    explicit DisplayImagesWidgetGui(QWidget *parent = 0);
    ~DisplayImagesWidgetGui();

  public slots:
    void display();

  private slots:
    void extractWords();
    void browseButton_clicked();
    void prevButton_clicked();
    void nextButton_clicked();

  private:
    Ui::DisplayImagesWidgetGui *ui;
    PageParser *m_pageParser;
    Reader *m_reader;
    int m_fileIndex;
    std::vector<QString> m_fNames;
    QString m_dir;
    std::vector<QString> m_words;     // file names are stored
    std::vector<QString> m_coords;     // file names are stored
};

#endif // DISPLAYIMAGESWIDGET_H
