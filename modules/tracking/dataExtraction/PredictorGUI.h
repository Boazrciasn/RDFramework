#ifndef PREDICTORGUI_H
#define PREDICTORGUI_H

#include <QWidget>

namespace Ui {
class PredictorGUI;
}

class PredictorGUI : public QWidget
{
    Q_OBJECT

public:
    explicit PredictorGUI(QWidget *parent = 0);
    ~PredictorGUI();

private:
    Ui::PredictorGUI *ui;
};

#endif // PREDICTORGUI_H
