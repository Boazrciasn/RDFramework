#include "precompiled.h"

#include "rdf/RDFParams.h"
#include "rdf/RandomDecisionForestDialogGui.h"
#include "ui_RandomDecisionForestDialogGui.h"


void RandomDecisionForestDialogGui::initParamValues()
{
    ui->spinBox_NTrees->setValue(PARAMS.nTrees);
    ui->spinBox_MaxDepth->setValue(PARAMS.maxDepth);
    ui->spinBox_probX->setValue(PARAMS.probDistX);
    ui->spinBox_probY->setValue(PARAMS.probDistY);
    ui->spinBox_PixelsPerImage->setValue(PARAMS.pixelsPerImage);
    ui->spinBox_MinLeafPixels->setValue(PARAMS.minLeafPixels);
    ui->spinBox_MaxIteration->setValue(PARAMS.maxIteration);
    ui->spinBox_LabelCount->setValue(PARAMS.labelCount);
    ui->spinBox_TauRange->setValue(PARAMS.tauRange);
    ui->checkBox_isPositive->setChecked(PARAMS.isPositiveRange);
    ui->spinBox_MaxPixelsEntropy->setValue(PARAMS.maxPxForEntropy);
    ui->checkBox_isMNIST->setChecked(PARAMS.isMNIST);
}

RandomDecisionForestDialogGui::RandomDecisionForestDialogGui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RandomDecisionForestDialogGui)
{
    ui->setupUi(this);
    readSettings();
    initParamValues();
    m_nTreesForDetection = ui->spinBox_NTestTrees->value();
    QObject::connect(&SignalSenderInterface::instance(), SIGNAL(printsend(QString)), this,
                     SLOT(printMsg(QString)));
    m_dataReaderGUI = new ReaderGUI();
    m_displayImagesGUI = new DisplayGUI();
    m_preprocessGUI = new PreProcessGUI();
    m_splitterVert = new QSplitter(this);
    m_splitterHori = new QSplitter(this);
    //Configure layout :
    m_splitterHori->addWidget(ui->LeftContainerWidget);
    m_splitterHori->addWidget(m_displayImagesGUI);
    m_splitterVert->addWidget(ui->tabWidget);
    m_splitterVert->addWidget(m_dataReaderGUI);
    m_splitterVert->addWidget(m_preprocessGUI);
    m_splitterVert->addWidget(ui->console);
    m_splitterVert->setOrientation(Qt::Vertical);
    QObject::connect(m_dataReaderGUI, SIGNAL(imagesLoaded(bool)), this, SLOT(onImagesLoaded(bool)));
    ui->verticalLayout_LeftContainer->addWidget(m_splitterVert);
    ui->gridLayout_RDFGUI->addWidget(m_splitterHori, 1, 0);
}

RandomDecisionForestDialogGui::~RandomDecisionForestDialogGui()
{
    delete m_dataReaderGUI;
    delete m_displayImagesGUI;
    delete &m_forest;
    delete &m_rdf;
    delete &m_preprocesses;
    delete m_preprocessGUI;
    delete m_splitterHori;
    delete m_splitterVert;
    delete ui;
}

void RandomDecisionForestDialogGui::onImagesLoaded(bool)
{
    m_displayImagesGUI->setImageSet(m_dataReaderGUI->DS()->images);

    auto lbl_count = m_dataReaderGUI->DS()->map_dataPerLabel.size();
    if(lbl_count == 0)
        lbl_count = 2;
    ui->spinBox_LabelCount->setValue(lbl_count);
    PARAMS.labelCount = lbl_count;
}

void RandomDecisionForestDialogGui::onLabelsLoaded()
{
    PARAMS.testDir = QFileDialog::getExistingDirectory(this, tr("Open Image Directory"), PARAMS.trainImagesDir,
                                                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->console->append("Test images read");
    ui->console->setText(PARAMS.testDir);
}

void RandomDecisionForestDialogGui::onTrain()
{
    m_forest.setParams(PARAMS);
    m_forest.setPreProcess(m_preprocessGUI->preprocesses());
    m_forest.setDataSet(m_dataReaderGUI->DS());
    QString dirname = QFileDialog::getExistingDirectory(this,
                                                        tr("Open Save Directory"), PARAMS.trainImagesDir,
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QString fname = "nT_" + QString::number(m_forest.params().nTrees) + "_D_" +
                    QString::number(m_forest.params().maxDepth)
                    //                    + "_nTImg_" + QString::number(m_forest.m_imagesContainer.size()) // TODO: add later
                    + "_nPxPI_" + QString::number(m_forest.params().pixelsPerImage)
                    + ".bin";
    qApp->processEvents();
    if (m_forest.trainForest())
    {
        printMsg("Forest Trained ! ");
        m_forest.saveForest(dirname + "/" + fname);
        ui->console->append("FOREST SAVED");
    }
    else
        printMsg("Failed to Train Forest! (Forest DS is not set) ");
}

void RandomDecisionForestDialogGui::onTest()
{
    if(ui->checkBox_isMNIST->isChecked())
    {
        if(ui->checkBox_isRDFC->isChecked())
            test(m_rdf_c);
        else
            test(m_rdf);
    }
    else
    {
        if(ui->checkBox_isRDFC->isChecked())
            testSingleFrame(m_rdf_c);
        else
            testSingleFrame(m_rdf);
    }
}

void RandomDecisionForestDialogGui::onPreProcess()
{
    PreProcess::doBatchPreProcess(m_dataReaderGUI->DS()->images, m_preprocessGUI->preprocesses());
    m_displayImagesGUI->setImageSet(m_dataReaderGUI->DS()->images);
    m_dataReaderGUI->DS()->isProcessed = true;
}

void RandomDecisionForestDialogGui::printMsg(QString msg)
{
    QString datetime = QDateTime::currentDateTime().toString("dd/MM/yy hh:mm");
    QString datetimeText = datetime + " ---- " + msg;
    ui->console->append(datetimeText);
    ui->console->repaint();
    std::cout<< datetimeText.toStdString()<< std::endl;
}

void RandomDecisionForestDialogGui::onLoad()
{
    QString selfilter = tr("BINARY (*.bin *.txt)");
    QString fname = QFileDialog::getOpenFileName(
                        this,
                        tr("Load Forest Directory"),
                        QDir::currentPath(),
                        tr("BINARY (*.bin);;TEXT (*.txt);;All files (*.*)"),
                        &selfilter
                    );

    if(ui->checkBox_isRDFC->isChecked())
         m_rdf_c.loadForest(fname);
    else
        m_rdf.loadForest(fname);
    ui->console->append("FOREST LOADED");
}

void RandomDecisionForestDialogGui::onSave()
{
    QString dirname = QFileDialog::getExistingDirectory(this,
                                                        tr("Open Save Directory"), PARAMS.trainImagesDir,
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QString fname = "nT_" + QString::number(m_forest.params().nTrees) + "_D_" +
                    QString::number(m_forest.params().maxDepth)
                    //                    + "_nTImg_" + QString::number(m_forest.m_imagesContainer.size()) // TODO: add later
                    + "_nPxPI_" + QString::number(m_forest.params().pixelsPerImage)
                    + ".bin";
    m_forest.saveForest(dirname + "/" + fname);
    ui->console->append("FOREST SAVED");
}

void RandomDecisionForestDialogGui::closeEvent(QCloseEvent *event)
{
    writeSettings();
    m_dataReaderGUI->writeSettings();
    event->accept();
}

void RandomDecisionForestDialogGui::writeSettings()
{
    QSettings settings("VVGLab", "RDFDialogGUI");
    settings.beginGroup("PARAMS");
    settings.setValue("nTrees", PARAMS.nTrees);
    settings.setValue("maxDepth", PARAMS.maxDepth);
    settings.setValue("probDistX", PARAMS.probDistX);
    settings.setValue("probDistY", PARAMS.probDistY);
    settings.setValue("pixelsPerImage", PARAMS.pixelsPerImage);
    settings.setValue("minLeafPixels", PARAMS.minLeafPixels);
    settings.setValue("maxIteration", PARAMS.maxIteration);
    settings.setValue("labelCount", PARAMS.labelCount);
    settings.setValue("tauRange", PARAMS.tauRange);
    settings.setValue("isPositiveRange", PARAMS.isPositiveRange);
    settings.setValue("maxPxForEntropy", PARAMS.maxPxForEntropy);
    settings.setValue("isMNIST", PARAMS.isMNIST);
    settings.endGroup();
}

void RandomDecisionForestDialogGui::readSettings()
{
    QSettings settings("VVGLab", "RDFDialogGUI");
    settings.beginGroup("PARAMS");
    PARAMS.nTrees = settings.value("nTrees", 12).toInt();
    PARAMS.maxDepth = settings.value("maxDepth", 20).toInt();
    PARAMS.probDistX = settings.value("probDistX", 30).toInt();
    PARAMS.probDistY = settings.value("probDistY", 30).toInt();
    PARAMS.pixelsPerImage = settings.value("pixelsPerImage", 200).toInt();
    PARAMS.minLeafPixels = settings.value("minLeafPixels", 5).toInt();
    PARAMS.maxIteration = settings.value("maxIteration", 100).toInt();
    PARAMS.labelCount = settings.value("labelCount", 26).toInt();
    PARAMS.tauRange = settings.value("tauRange", 127).toInt();
    PARAMS.isPositiveRange = settings.value("isPositiveRange", false).toBool();
    PARAMS.maxPxForEntropy = settings.value("maxPxForEntropy", 1000).toInt();
    PARAMS.isMNIST = settings.value("isMNIST", false).toBool();
    settings.endGroup();
}

void RandomDecisionForestDialogGui::onNTreesChanged(int value)
{
    PARAMS.nTrees = value;
    ui->spinBox_NTestTrees->setMaximum(value);
}

void RandomDecisionForestDialogGui::onNTestTreesChanged(int value)
{
    m_nTreesForDetection = value;
}

void RandomDecisionForestDialogGui::onMaxDepthChanged(int value)
{
    PARAMS.maxDepth = value;
}

void RandomDecisionForestDialogGui::onProbDistXChanged(int value)
{
    PARAMS.probDistX = value;
}

void RandomDecisionForestDialogGui::onProbDistYChanged(int value)
{
    PARAMS.probDistY = value;
}

void RandomDecisionForestDialogGui::onPixelsPerImageChanged(int value)
{
    PARAMS.pixelsPerImage = value;
}

void RandomDecisionForestDialogGui::onMinLeafPixelsChanged(int value)
{
    PARAMS.minLeafPixels = value;
}

void RandomDecisionForestDialogGui::onMaxIterationChanged(int value)
{
    PARAMS.maxIteration = value;
}

void RandomDecisionForestDialogGui::onLabelCountChanged(int value)
{
    PARAMS.labelCount = value;
}

void RandomDecisionForestDialogGui::onTauRangeChanged(int value)
{
    PARAMS.tauRange = value;
}

void RandomDecisionForestDialogGui::onPositiveTau(bool value)
{
    PARAMS.isPositiveRange = value;
}

void RandomDecisionForestDialogGui::onMNIST(bool value)
{
    PARAMS.isMNIST = value;
}

