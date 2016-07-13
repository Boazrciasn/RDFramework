#ifndef PFWIDGET_H
#define PFWIDGET_H
#include "src/precompiled.h"

#include <QWidget>

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFileDialog>
#include <QProgressBar>
#include <QSlider>

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
    void on_actionOpen_triggered();

private:
    Ui::PFWidget *ui;
    QMediaPlayer* mPlayer;
    QVideoWidget* mVideoWidg;
    QProgressBar* mProgressBar;
    QSlider* mSlider;
};

#endif // PFWIDGET_H
