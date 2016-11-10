#ifndef PARTICLEFILTERWIDGETGUI_H
#define PARTICLEFILTERWIDGETGUI_H
#include "precompiled.h"

#include <QWidget>

#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <tuple>
#include "Util.h"
#include "tracking/videoplayer/VideoReader.h"
#include "tracking/videoplayer/VideoPlayer.h"
#include "tracking/particlefilter/ParticleFilter.h"
#include "Target.h"
#include "tracking/dataExtraction/PredictorGui.h"
#include <opencv2/objdetect.hpp>

class RectangleParticle;

namespace Ui
{
class ParticleFilterWidgetGui;
}

class ParticleFilterWidgetGui : public QWidget
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

  public:
    explicit ParticleFilterWidgetGui(QWidget *parent = 0);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    inline void setSVM(cv::Ptr<cv::ml::SVM> svm){m_svm = svm;}
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
    void onActionDispConfMap();

    void onParticleCountChange(int value);
    void onIterationCountChange(int value);
    void onParticleWidthChange(int value);
    void onParticleHeightChange(int value);
    void onHistSizeChanged(int value);
    void onStepSizeChanged(int value);

  private:
    Ui::ParticleFilterWidgetGui *ui;
    VideoPlayer *m_VideoPlayer;
    QString m_videoFile{};
    QPixmap m_originalPix;
    PredictorGui *m_predictor{};
    ParticleFilter *m_PF{};
    cv::Ptr<cv::ml::SVM> m_svm;

    cv::Mat m_Frame;
    cv::Mat m_FrameGray;
    cv::Mat m_FrameOut;
    int m_particleCount;
    int m_numIters;
    int m_particleWidth;
    int m_particleHeight;
    int m_histSize ;
    int m_stepSize;
    bool m_VideoLodaded{};
    bool m_isPlaying{};
    bool m_dragging{};
    QRubberBand *m_RubberBand;
    QRect m_TargetROI;
    QPoint m_Point;
    QPoint m_PointMove;

    RectangleParticle *m_RectParticle;

    QVector<Target *> m_TargetsVector;
    quint32 m_TargetCount = 0 ;
    int m_currFrame;

    void writeSettings();
    void readSettings();
    void setPFSettingsEnabled(bool state);
    void setConfidence();
    void loadVideo(QString filename);
};

#endif // PARTICLEFILTERWIDGETGUI_H