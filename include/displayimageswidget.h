#ifndef DISPLAYIMAGESWIDGET_H
#define DISPLAYIMAGESWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QFileDialog>
#include <QLabel>
#include <QImage>
#include <QThread>

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
    void browseButton_clicked();
    void prevButton_clicked();
    void nextButton_clicked();

  private:
    Ui::DisplayImagesWidget *ui;
    Reader *m_reader;
    int m_fileIndex;
    std::vector<QString> m_fNames;
    QString m_dir;
};

#endif // DISPLAYIMAGESWIDGET_H
