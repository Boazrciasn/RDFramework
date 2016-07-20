#ifndef PFWIDGET_H
#define PFWIDGET_H
#include "src/precompiled.h"

#include <QWidget>

#include <QFileDialog>
#include <QMessageBox>
#include "videoplayer.h"
#include "SimplePF.h"

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
    void updatePlayerUI(QImage img);
    QString getFormattedTime(int timeInSeconds);
    void onHorizontalSliderPressed();
    void onHorizontalSliderReleased();
    void onHorizontalSliderMoved(int position);
    void onActionOpen();
    void onActionPlay();
    void onActionPause();
    void onActionStop();
    void processFrame(QImage image);

private:
    Ui::PFWidget *ui;
    videoPlayer *myPlayer;
    SimplePF *mPF;

    cv::Mat mFrame;
    cv::Mat mFrameGray;
    cv::Mat mFrameOut;
    int nParticles;
    int nIters;

};

#endif // PFWIDGET_H
