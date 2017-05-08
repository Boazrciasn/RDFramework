#ifndef CAMCALIBGUI_H
#define CAMCALIBGUI_H

#include <QWidget>
#include <QSplitter>

#include "modules/tracking/videoplayer/VideoPlayerGui.h"
#include "Core/DisplayGUI.h"

namespace Ui
{
class CamCalibGUI;
}

class CamCalibGUI : public QWidget
{
    Q_OBJECT

  public:
    explicit CamCalibGUI(QWidget *parent = 0);
    ~CamCalibGUI();

  private:
    Ui::CamCalibGUI *ui;
    VideoPlayerGui *m_videoplayer{};
    DisplayGUI *m_displayImagesGUI;
    QSplitter *m_splitterVert{};
    QSplitter *m_splitterHori{};
    cv::Mat m_img;
    double alpha, beta, gamma, f, d;

  private slots:
    void applyIPM(QImage img);
};

#endif // CAMCALIBGUI_H
