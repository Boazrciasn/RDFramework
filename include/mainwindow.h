#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QFileDialog>
#include <QLabel>
#include <QImage>
#include <QThread>
#include "include/Reader.h"
#include "include/mydialog.h"
#include "include/PageParser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void update();

private slots:
    void on_browse_clicked();
    void on_previous_clicked();
    void on_next_clicked();

private:
    void setLable();

    Ui::MainWindow *ui;
    MyDialog *mDialog;
    Reader *reader;
    QString dir;                // file directory
    vector<QString> fNames;     // file names are stored
    int fileIndex;              // index of file being viewed

    PageParser *pageParser;
    vector<QString> words;     // file names are stored
    vector<QString> coords;     // file names are stored

    int process_type = -1;      // 0 for IMG / 1 for video
    int nParticles = 100;
    int nIters = 10;
};

#endif // MAINWINDOW_H
