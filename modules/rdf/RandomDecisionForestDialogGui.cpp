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
    delete &m_forest_basic;
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
    if (m_forest.trainForest())
        printMsg("Forest Trained ! ");
    else
        printMsg("Failed to Train Forest! (Forest DS is not set) ");
}

void RandomDecisionForestDialogGui::onTest()
{
//    m_forest_basic.setNTreesForDetection(m_nTreesForDetection);
    auto begin = std::chrono::high_resolution_clock::now();
//    auto accuracy = m_forest.testForest();

    // ///////////////////////////////////////////////////
    // ///////////////////////////////////////////////////
    // New Test
    auto size = m_dataReaderGUI->DS()->images.size();
    std::vector<cv::Mat> results{};
    Colorcode colorsBag;


    std::atomic<int> posCounter(0);
    auto pxAcc = 0.0f;
    for(auto i = 0u; i <  size; ++i)
    {
        cv::Mat labels{};
        cv::Mat_<float> confs{};
        cv::Mat_<float> layered = m_forest_basic.getLayeredHist(m_dataReaderGUI->DS()->images[i]);
        m_forest_basic.getLabelAndConfMat(layered, labels, confs);

        results.push_back(m_dataReaderGUI->DS()->images[i]);
        results.push_back(labels);

        auto color = colorsBag.colors[m_dataReaderGUI->DS()->labels[i]];
        auto counter = 0.0f;
        auto c = 0.0f;
        for (int row = 0; row < labels.rows; ++row) {
            for (int col = 0; col < labels.cols; ++col) {
                if(labels.at<cv::Vec3b>(row,col) == cv::Vec3b(255,255,255))
                    continue;
                ++c;
                if (labels.at<cv::Vec3b>(row,col) == color)
                    ++counter;
            }
        }

        pxAcc += counter/c;
        int label{};
        float conf{};
        m_forest_basic.detect(m_dataReaderGUI->DS()->images[i], label, conf);
        if (label == m_dataReaderGUI->DS()->labels[i])
            ++posCounter;
    }

    m_displayImagesGUI->setImageSet(results);
    // ///////////////////////////////////////////////////
    // ///////////////////////////////////////////////////

    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::seconds>(end-begin).count();

    printMsg("Testing time: " + QString::number(time) + QString(" sec  ") + QString::number(time/60) + QString(" min"));
    printMsg("Accuracy: " + QString::number(100 * (float) posCounter / (float)size) + "%");
    printMsg("Pixel Accuracy: " + QString::number(100 * (float) pxAcc / (float)size) + "%");
}

void RandomDecisionForestDialogGui::onPreProcess()
{
    PreProcess::doBatchPreProcess(m_dataReaderGUI->DS()->images, m_preprocessGUI->preprocesses());
    m_displayImagesGUI->setImageSet(m_dataReaderGUI->DS()->images);
    m_dataReaderGUI->DS()->isProcessed = true;
}

void RandomDecisionForestDialogGui::printMsg(QString msg)
{
    ui->console->append(msg);
    ui->console->repaint();
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
    m_forest_basic.loadForest(fname);
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
