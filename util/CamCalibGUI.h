#ifndef CAMCALIBGUI_H
#define CAMCALIBGUI_H

#include <QWidget>
#include <QSplitter>

#include "modules/tracking/videoplayer/VideoPlayerGui.h"
#include "Core/DisplayGUI.h"
#include "util/IPMMapper.h"
#define PI 3.1415926
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
    cv::Mat m_transformedImg;
    HomographyParams m_params;

  private slots:
    void collectImage(QImage img);
    void alphaslider(int inpt);
    void betaslider(int inpt);
    void gammaslider(int inpt);
    void dslider(int inpt);
    void fslider(int inpt);
};

#endif // CAMCALIBGUI_H
