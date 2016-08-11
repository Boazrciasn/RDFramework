#include "precompiled.h"

#include "Core/MainWindowGui.h"

#include "ocr/TextRegionDetector.h"
#include "ui_MainWindowGui.h"



MainWindowGui::MainWindowGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowGui)
{
    ui->setupUi(this);
    // Shortcut to Random Desicion Forest Window
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    QObject::connect(shortcut, SIGNAL(activated()), this,
                     SLOT(on_actionRDF_triggered()));
    shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
    QObject::connect(shortcut, SIGNAL(activated()), this,
                     SLOT(on_actionResizer_triggered()));
}

MainWindowGui::~MainWindowGui()
{
    std::cout << "quitting application." << std::endl;
    delete ui;
}

void MainWindowGui::on_actionAnnotation_Extractor_triggered()
{
    AnnExtractorDialogGui *mDialog = new AnnExtractorDialogGui(this);
    mDialog->exec();
}

void MainWindowGui::on_actionRDF_triggered()
{
    RandomDecisionForestDialogGui *mDialog = new RandomDecisionForestDialogGui(this);
    mDialog->exec();
}

void MainWindowGui::on_actionResizer_triggered()
{
    ResizeAllImagesDialogGui *mDialog = new ResizeAllImagesDialogGui(this);
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
