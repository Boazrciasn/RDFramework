#ifndef DATAEXTRACTORGUI_H
#define DATAEXTRACTORGUI_H

#include <QWidget>
#include "DisplayImagesWidgetGui.h"
#include "BackgroundSubtractors.h"
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
    DataExtractor<BackgroundSubMoG, int> *m_dataExtractMoG;
    BackgroundSubMoG m_bgsubMoG;
    Ui::DataExtractorGui *ui;
    VideoPlayerGui *m_testWin;
};

#endif // DATAEXTRACTORGUI_H
