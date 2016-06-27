#include "precompiled.h"

#include "include/mainwindow.h"

#include "include/TextRegionDetector.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

MainWindow::~MainWindow()
{
    std::cout << "quitting application." << std::endl;
    delete ui;
}

void MainWindow::on_actionAnnotation_Extractor_triggered()
{
    AnnExtractorDialog *mDialog = new AnnExtractorDialog(this);
    mDialog->exec();
}

void MainWindow::on_actionRDF_triggered()
{
    RDFDialog *mDialog = new RDFDialog(this);
    mDialog->exec();
}

void MainWindow::on_actionResizer_triggered()
{
    ResizeAllImagesDialog *mDialog = new ResizeAllImagesDialog(this);
    mDialog->exec();
}

void MainWindow::open_displayimageswidget()
{
    DisplayImagesWidget *displayimageswidget = new DisplayImagesWidget();
    displayimageswidget->show();
}

void MainWindow::open_RDFwidget()
{
    RDFDialog *rdfwidget = new RDFDialog();
    rdfwidget->show();
}

void MainWindow::open_AnnExtwidget()
{
    AnnExtractorDialog *annextwidget = new AnnExtractorDialog();
    annextwidget->show();
}

void MainWindow::open_Resizerwidget()
{
    ResizeAllImagesDialog *resizerwidget = new ResizeAllImagesDialog();
    resizerwidget->show();
}
