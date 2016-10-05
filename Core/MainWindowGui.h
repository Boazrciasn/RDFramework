#ifndef MAINWINDOWGUI_H
#define MAINWINDOWGUI_H

#include <QMainWindow>
#include <QScrollArea>
#include <QFileDialog>
#include <QLabel>
#include <QImage>
#include <QThread>

#include "util/Reader.h"
#include "ocr/MyDialogGui.h"
#include "ocr/HistogramDialogGui.h"
#include "Util.h"
#include "rdf/RandomDecisionForestDialogGui.h"
#include "ocr/AnnExtractorDialogGui.h"
#include "Core/ResizeAllImagesDialogGui.h"
#include "Core/DisplayImagesWidgetGui.h"
#include "tracking/particlefilter/ParticleFilterWidgetGui.h"
#include "tracking/dataExtraction/DataExtractorGui.h"


namespace Ui
{
class MainWindowGui;
}

class MainWindowGui : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindowGui(QWidget *parent = 0);
    ~MainWindowGui();

  private slots:
    void on_actionAnnotation_Extractor_triggered();
    void on_actionRDF_triggered();
    void on_actionResizer_triggered();
    void open_displayimageswidget();
    void open_RDFwidget();
    void open_AnnExtwidget();
    void open_Resizerwidget();
    void open_PFTracker();
    void open_DataExtractor();

  private:
    Ui::MainWindowGui *ui;
    MyDialogGui *mDialog;
    HistogramDialogGui *histDialog;
};

#endif // MAINWINDOWGUI_H
