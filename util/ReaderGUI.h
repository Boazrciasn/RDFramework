#ifndef READERGUI_H
#define READERGUI_H
#include "precompiled.h"
#include "Reader.h"
#include "modules/rdf/DataSet.h"
#include <QWidget>

namespace Ui
{
class ReaderGUI;
}

class ReaderGUI : public QWidget
{
    Q_OBJECT

  public:
    explicit ReaderGUI(QWidget *parent = 0);
    DataSet *DS() { return m_DS;}
    void writeSettings();
    ~ReaderGUI();

  private:
    Ui::ReaderGUI *ui;
    DataSet *m_DS;
    Reader *m_reader;
    int m_dFlag;

    QString m_fileDir;
    void readSettings();


    void countLabels();
    void loadFrames(QString dir, std::vector<cv::Mat> &images, QVector<QVector<QRect> > &frameRects);
    void loadTME(QString dir, std::vector<cv::Mat> &images, QVector<QVector<QRect> > &frameRects);
    void loadMOT(QString dir, std::vector<cv::Mat> &images, QVector<QVector<QRect> > &frameRects);

private slots:
    void load();
    void dataTypeChanged(int index);

  signals:
    void imagesLoaded(bool load);
    void labelsLoaded(bool load);
};

#endif // READERGUI_H
