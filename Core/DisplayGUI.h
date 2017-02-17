#ifndef DISPLAYGUI_H
#define DISPLAYGUI_H
#include "precompiled.h"
#include <QWidget>

namespace Ui
{
class DisplayGUI;
}

class DisplayGUI : public QWidget
{
    Q_OBJECT

  public:
    explicit DisplayGUI(QWidget *parent = 0);
    void setImageSet(std::vector<cv::Mat> &images);
    ~DisplayGUI();

  private:
    Ui::DisplayGUI *ui;
    int m_fileIndex = 0;
    bool m_labelsExists;
    bool m_imagesExists;
    std::vector<cv::Mat> m_images;
    void display();

  private slots:
    void onSliderMove(int value);


};

#endif // DISPLAYGUI_H
