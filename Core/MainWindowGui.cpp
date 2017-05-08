#include "precompiled.h"

#include "Core/MainWindowGui.h"

#include "ocr/TextRegionDetector.h"
#include "ui_MainWindowGui.h"
#include "modules/tracking/dataExtraction/HOGExtactor.h"


MainWindowGui::MainWindowGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowGui)
{
    ui->setupUi(this);
    // Shortcut to Random Desicion Forest Window
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(on_actionRDF_triggered()));
    shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(on_actionResizer_triggered()));
}

MainWindowGui::~MainWindowGui()
{
    std::cout << "quitting application." << std::endl;
    delete ui;
}

void MainWindowGui::on_actionAnnotation_Extractor_triggered()
{
    AnnExtractorDialogGui *mDialog = new AnnExtractorDialogGui();
    mDialog->exec();
}

void MainWindowGui::on_actionRDF_triggered()
{
    RandomDecisionForestDialogGui *mDialog = new RandomDecisionForestDialogGui();
    mDialog->exec();
}

void MainWindowGui::on_actionResizer_triggered()
{
    ResizeAllImagesDialogGui *mDialog = new ResizeAllImagesDialogGui();
    mDialog->exec();
}

void MainWindowGui::open_displayimageswidget()
{
    DisplayImagesWidgetGui *displayimageswidget = new DisplayImagesWidgetGui();
    displayimageswidget->show();
}

void MainWindowGui::open_RDFwidget()
{
    RandomDecisionForestDialogGui *rdfwidget = new RandomDecisionForestDialogGui();
    rdfwidget->show();
}

void MainWindowGui::open_AnnExtwidget()
{
    AnnExtractorDialogGui *annextwidget = new AnnExtractorDialogGui();
    annextwidget->show();
}

void MainWindowGui::open_Resizerwidget()
{
    ResizeAllImagesDialogGui *resizerwidget = new ResizeAllImagesDialogGui();
    resizerwidget->show();
}

void MainWindowGui::open_PFTracker()
{
    ParticleFilterWidgetGui *pfwidget = new ParticleFilterWidgetGui();
    pfwidget->show();
}

void MainWindowGui::open_DataExtractor()
{
    DataExtractorGui *dataExtractwidget = new DataExtractorGui();
    dataExtractwidget->show();
}

void MainWindowGui::open_CamCalib()
{
    CamCalibGUI *camcalibwidget = new CamCalibGUI();
    camcalibwidget->show();
}

void MainWindowGui::open_HOGExtractor()
{
    HOGExtactor *hogExtr = new HOGExtactor();
    cv::Mat result = hogExtr->getResult();

    if(result.rows > 0)
    {
        QString fileName = QFileDialog::getSaveFileName(this, QObject::tr("Save File"),
                                                        "/home/posDes.yml",QObject::tr("Features Mat (*.yml)"));
        cv::FileStorage file(fileName.toStdString(), cv::FileStorage::WRITE);
        file << "data" << result;
        file.release();
    }

    delete hogExtr;
}
