#ifndef PFWIDGET_H
#define PFWIDGET_H
#include "src/util/precompiled.h"

#include <QWidget>

#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <tuple>
#include "Util.h"
#include "VideoReader.h"
#include "particlefilter/ParticleFilter.h"
#include "VideoPlayer.h"
#include "Target.h"

namespace Ui
{
class PFWidget;
}

class PFWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit PFWidget(QWidget *parent = 0);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
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
    void onActionSaveTarget();
    void onActionSetupPF();

    void onParticleCountChange(int value);
    void onIterationCountChange(int value);
    void onParticleWidthChange(int value);
    void onHistSizeChanged(int value);

  private:
    Ui::PFWidget *ui;
    VideoPlayer *m_VideoPlayer;
    ParticleFilter *m_PF;

    cv::Mat m_Frame;
    cv::Mat m_FrameGray;
    cv::Mat m_FrameOut;
    int m_nParticles;
    int m_nIters;
    int m_ParticleWidth;
    int m_histSize = 64;
    bool m_VideoLodaded = false;
    QRubberBand *m_RubberBand;
    QRect m_TargetROI;
    QPoint m_Point;

    RectangleParticle *m_RectParticle;

    QVector<Target *> m_TargetsVector;
    quint32 m_TargetCount = 0 ;

    void setPFSettingsEnabled(bool state);
};

#endif // PFWIDGET_H
