#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QFileDialog>
#include <QLabel>
#include <QImage>
#include <QThread>

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
    void on_pushButton_clicked();

private:
    void setLable();

    Ui::MainWindow *ui;

    int process_type = -1;      // 0 for IMG / 1 for video
    int nParticles = 100;
    int nIters = 10;
};

#endif // MAINWINDOW_H
