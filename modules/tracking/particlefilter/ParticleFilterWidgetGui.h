#ifndef PFWIDGET_H
#define PFWIDGET_H
#include "precompiled.h"

#include <QWidget>

#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <tuple>
#include "Util.h"
#include "tracking/videoplayer/VideoReader.h"
#include "tracking/particlefilter/ParticleFilter.h"
#include "tracking/videoplayer/VideoPlayer.h"
#include "Target.h"

class RectangleParticle;

namespace Ui
{
class ParticleFilterWidgetGui;
}

class ParticleFilterWidgetGui : public QWidget
{
    Q_OBJECT

  public:
    explicit ParticleFilterWidgetGui(QWidget *parent = 0);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    ~ParticleFilterWidgetGui();

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
    void onActionSaveTarget();
    void onActionSetupPF();

    void onParticleCountChange(int value);
    void onIterationCountChange(int value);
    void onParticleWidthChange(int value);
    void onParticleHeightChange(int value);
    void onHistSizeChanged(int value);

  private:
    Ui::ParticleFilterWidgetGui *ui;
    VideoPlayer *m_VideoPlayer;
    ParticleFilter *m_PF{};

    cv::Mat m_Frame;
    cv::Mat m_FrameGray;
    cv::Mat m_FrameOut;
    int m_particleCount;
    int m_numIters;
    int m_particleWidth;
    int m_particleHeight;
    int m_histSize ;
    bool m_VideoLodaded = false;
    bool m_isPlaying;
    QRubberBand *m_RubberBand;
    QRect m_TargetROI;
    QPoint m_Point;

    RectangleParticle *m_RectParticle;

    QVector<Target *> m_TargetsVector;
    quint32 m_TargetCount = 0 ;
    int m_currFrame;

    void setPFSettingsEnabled(bool state);
};

#endif // PFWIDGET_H
