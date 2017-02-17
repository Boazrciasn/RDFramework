#ifndef DISPLAYGUI_H
#define DISPLAYGUI_H

#include <QWidget>

namespace Ui {
class DisplayGUI;
}

class DisplayGUI : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayGUI(QWidget *parent = 0);
    ~DisplayGUI();

private:
    Ui::DisplayGUI *ui;
};

#endif // DISPLAYGUI_H
