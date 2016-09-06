#ifndef VIDEOPLAYERGUI_H
#define VIDEOPLAYERGUI_H

#include <QWidget>

#include "tracking/videoplayer/VideoReader.h"
#include "tracking/videoplayer/VideoPlayer.h"

namespace Ui
{
class VideoPlayerGui;
}

class VideoPlayerGui : public QWidget
{
    Q_OBJECT

  public:
    explicit VideoPlayerGui(QWidget *parent = 0);
    ~VideoPlayerGui();

  private:
    Ui::VideoPlayerGui *ui;
    VideoPlayer *m_VideoPlayer;
    bool m_VideoLodaded = false;
    bool m_isPlaying;
    int m_currFrame;

  private slots :
    void updatePlayerUI(QImage img);
    void onHorizontalSliderPressed();
    void onHorizontalSliderReleased();
    void onHorizontalSliderMoved(int position);
    void onActionBrowse();
    void onActionPlay();
    void onActionPause();
};

#endif // VIDEOPLAYERGUI_H
