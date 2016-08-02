#ifndef PFWIDGET_H
#define PFWIDGET_H
#include "src/precompiled.h"

#include <QWidget>

#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include "Util.h"
#include "VideoReader.h"
#include "SimplePF.h"
#include "PFExecutor.h"
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

    void onParticleCountChange(int value);
    void onIterationCountChange(int value);
    void onParticleWidthChange(int value);

  private:
    Ui::PFWidget *ui;
    PFExecutor *m_PF_executor;
    SimplePF *mPF;

    cv::Mat mFrame;
    cv::Mat mFrameGray;
    cv::Mat mFrameOut;
    int nParticles;
    int nIters;
    int mParticleWidth;
    bool m_VideoLodaded = false;
    QRubberBand *m_RubberBand;
    QRect m_TargetROI;
    QPoint m_Point;

    QVector<Target *> m_TargetsVector;
    quint32 m_TargetCount = 0 ;

    void setPFSettingsEnabled(bool state);
};

#endif // PFWIDGET_H
