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
    inline std::vector<Process*> preprocesses()
    {
        createPreProcesses();
        return m_processes;
    }
    ~PreProcessGUI();

  private slots:
    void onKernelChanged();

  private:
    enum ProcessTypes
    {
        INVERSE_IMG,
        MAKEBORDER,
        GAUSS
    };
    void createPreProcesses();
    Ui::PreProcessGUI *ui;
    //List of PreProcess types :
    std::vector<Process*> m_processes;


};

#endif // PREPROCESSGUI_H
