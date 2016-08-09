#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QFileDialog>
#include <QLabel>
#include <QImage>
#include <QThread>

#include "Reader.h"
#include "MyDialog.h"
#include "HistogramDialog.h"
#include "Util.h"
#include "RDFDialog.h"
#include "AnnExtractorDialog.h"
#include "ResizeAllImagesDialog.h"
#include "DisplayImagesWidget.h"
#include "PFWidget.h"


namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

  private slots:
    void on_actionAnnotation_Extractor_triggered();
    void on_actionRDF_triggered();
    void on_actionResizer_triggered();
    void open_displayimageswidget();
    void open_RDFwidget();
    void open_AnnExtwidget();
    void open_Resizerwidget();
    void open_PFTracker();

  private:
    Ui::MainWindow *ui;
    MyDialog *mDialog;
    HistogramDialog *histDialog;
};

#endif // MAINWINDOW_H
