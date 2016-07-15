#ifndef PFWIDGET_H
#define PFWIDGET_H
#include "src/precompiled.h"

#include <QWidget>

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFileDialog>
#include <QProgressBar>
#include <QSlider>
#include <videoframegrabber.h>

namespace Ui {
class PFWidget;
}

class PFWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PFWidget(QWidget *parent = 0);
    ~PFWidget();

private slots:
    void onActionOpen();
    void onActionPlay();
    void onActionPause();
    void onActionStop();
    void processFrame(QImage image);

private:
    Ui::PFWidget *ui;
    QMediaPlayer* mPlayer;
    VideoFrameGrabber* mVideoWidg;
    QProgressBar* mProgressBar;
    QSlider* mSlider;
};

#endif // PFWIDGET_H
