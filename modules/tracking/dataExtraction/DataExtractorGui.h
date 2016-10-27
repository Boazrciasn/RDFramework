#ifndef DATAEXTRACTORGUI_H
#define DATAEXTRACTORGUI_H

#include <QWidget>
#include "DisplayImagesWidgetGui.h"
#include "PredictorGui.h"
#include "BackgroundSubtractors.h"
#include "tracking/videoplayer/VideoPlayerGui.h"
#include <opencv2/objdetect.hpp>

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
    void spinBoxesUpdate();

  private:
    DataExtractor<BackgroundSubMoG, int> *m_dataExtractMoG;
    BackgroundSubMoG m_bgsubMoG;

    Ui::DataExtractorGui *ui;
    VideoPlayerGui *m_testWin;
    PredictorGui *m_predictSet;
};

#endif // DATAEXTRACTORGUI_H
