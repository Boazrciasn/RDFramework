#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include <QWidget>
#include "DisplayImagesWidgetGui.h"
#include "tracking/videoplayer/VideoPlayerGui.h"

namespace Ui
{
class dataExtractor;
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
    Ui::dataExtractor *ui;
    VideoPlayerGui *m_testWin;
};

#endif // DATAEXTRACTOR_H
