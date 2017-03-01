#ifndef PREPROCESSGUI_H
#define PREPROCESSGUI_H

#include <QWidget>
#include "rdf/PreProcess.h"


namespace Ui
{
class PreProcessGUI;
}

class PreProcessGUI : public QWidget
{
    Q_OBJECT

  public:
    explicit PreProcessGUI(QWidget *parent = 0);
    ~PreProcessGUI();

  private:
    enum ProcessTypes
    {
        INVERSE_IMG,
        MAKEBORDER,
        GAUSS
    };
    Ui::PreProcessGUI *ui;
    //List of PreProcess types :
    std::vector<Process> m_processList(InverseImage(), Gaussian(), MakeBorder());
    //Pre processses in order :
    std::vector<Process> m_preprocesses;

};

#endif // PREPROCESSGUI_H
