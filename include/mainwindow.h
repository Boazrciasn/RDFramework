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
#include "histogramdialog.h"
#include "include/PageParser.h"
#include "include/Util.h"
#include "rdfdialog.h"
#include "annextractordialog.h"
#include "resizeallimagesdialog.h"
#include "displayimageswidget.h"


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

  public slots:
    void display();

  private slots:
    void on_browse_clicked();
    void on_previous_clicked();
    void on_next_clicked();
    void on_extractWords_clicked();
    void on_actionAnnotation_Extractor_triggered();
    void on_actionRDF_triggered();
    void on_actionResizer_triggered();
    void open_displayimageswidget();
    void open_RDFwidget();
    void open_AnnExtwidget();
    void open_Resizerwidget();

  private:
    Ui::MainWindow *ui;
    void setLabel();
    int m_pageindex = 0;
    MyDialog *mDialog;
    HistogramDialog *histDialog;
    Reader *reader;
    QString dir;                // file directory
    std::vector<QString> fNames;     // file names are stored
    int fileIndex;              // index of file being viewed
    PageParser *pageParser;
    std::vector<QString> words;     // file names are stored
    std::vector<QString> coords;     // file names are stored
    int process_type = -1;      // 0 for IMG / 1 for video
    int nParticles = 100;
    int nIters = 10;
};

#endif // MAINWINDOW_H
