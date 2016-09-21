#ifndef DATAEXTRACTORGUI_H
#define DATAEXTRACTORGUI_H

#include <QWidget>
#include "DisplayImagesWidgetGui.h"
#include "tracking/videoplayer/VideoPlayerGui.h"

namespace Ui
{
class DataExtractorGui;
}

class DataExtractorGui : public QWidget
{
    Q_OBJECT

  public:
    explicit DataExtractorGui(QWidget *parent = 0);
    ~DataExtractorGui();

  private slots :
    void createNewWindow();

  private:
    Ui::DataExtractorGui *ui;
    VideoPlayerGui *m_testWin;
};

#endif // DATAEXTRACTORGUI_H
