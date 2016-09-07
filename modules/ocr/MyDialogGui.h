#ifndef MYDIALOGGUI_H
#define MYDIALOGGUI_H

#include <QDialog>
#include <vector>
#include "3rdparty/qcustomplot/qcustomplot.h" // the header file of QCustomPlot. Don't forget to add it to your project, if you use an IDE, so it gets compiled.

namespace Ui
{
class MyDialogGui;
}

class MyDialogGui : public QDialog
{
    Q_OBJECT

  public:
    explicit MyDialogGui(QWidget *parent = 0);
    void setLabel(QImage &img);
    void setFNames(std::vector<QString> &fNames, QString dir);
    ~MyDialogGui();

  private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_comboBox_activated(int index);

  private:
    Ui::MyDialogGui *ui;
    std::vector<QString> fNames;     // file names are stored
    int fileIndex;              // index of file being viewed
    QString dir;
};

#endif // MYDIALOGGUI_H

