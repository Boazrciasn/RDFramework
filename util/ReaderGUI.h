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
    ~ReaderGUI();

  private:
    Ui::ReaderGUI *ui;
    DataSet *m_DS;
    Reader *m_reader;
    int m_dFlag;


  private slots:
    void load();
    void dataTypeChanged(int index);

  signals:
    void imagesLoaded(bool load);
    void labelsLoaded(bool load);
};

#endif // READERGUI_H
