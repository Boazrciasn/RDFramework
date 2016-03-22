#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>
#include <vector>
#include "qcustomplot/qcustomplot.h" // the header file of QCustomPlot. Don't forget to add it to your project, if you use an IDE, so it gets compiled.

namespace Ui {
class MyDialog;
}

class MyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MyDialog(QWidget *parent = 0);
    void setLabel(QImage &img);
    void setFNames(std::vector<QString> &fNames, QString dir);
    ~MyDialog();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_comboBox_activated(int index);

private:
    Ui::MyDialog *ui;
    std::vector<QString> fNames;     // file names are stored
    int fileIndex;              // index of file being viewed
    QString dir;
};

#endif // MYDIALOG_H

