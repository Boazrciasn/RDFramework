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
    void onSetParticleCountSliderMoved(int position);

    void onActionOpen();
    void onActionPlay();
    void onActionPause();
    void onActionStop();

    void onParticleCountChange(int value);
    void onIterationCountChange(int value);
    void onParticleWidthChange(int value);

private:
    Ui::PFWidget *ui;
    VideoPlayer *myPlayer;
    SimplePF *mPF;

    cv::Mat mFrame;
    cv::Mat mFrameGray;
    cv::Mat mFrameOut;
    int nParticles;
    int nIters;
    int mParticleWidth;

};

#endif // PFWIDGET_H
