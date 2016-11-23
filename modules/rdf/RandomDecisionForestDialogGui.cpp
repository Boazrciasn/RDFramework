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
}

RandomDecisionForestDialogGui::RandomDecisionForestDialogGui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RandomDecisionForestDialogGui)
{
    ui->setupUi(this);
    readSettings();
    initParamValues();


    QObject::connect(&m_forest, SIGNAL(treeConstructed()), this, SLOT(new_tree_constructed()));
//    QObject::connect(&m_forest, SIGNAL(classifiedImageAs(int, char)), this, SLOT(image_at_classified_as(int, char)));
//    QObject::connect(&m_forest, SIGNAL(resultPercentage(double)), this, SLOT(resultPercetange(double))) ;

    // TODO: create corresponding slots
//    QObject::connect(m_readerGUI,SIGNAL(imagesLoaded(bool)), this, SLOT(imagesLoaded(bool)));
//    QObject::connect(m_readerGUI,SIGNAL(labelsLoaded(bool)), this, SLOT(labelsLoaded(bool)));
    m_trainDataReaderGUI = new ReaderGUI();
    m_testDataReaderGUI = new ReaderGUI();
    ui->gridLayout_train->addWidget(m_trainDataReaderGUI,2,0,3,4);
    ui->gridLayout_test->addWidget(m_testDataReaderGUI,2,0,3,2);
}

RandomDecisionForestDialogGui::~RandomDecisionForestDialogGui()
{
    delete ui;
}

void RandomDecisionForestDialogGui::onTrainingBrowse()
{
    PARAMS.trainImagesDir = QFileDialog::getExistingDirectory(this, tr("Open Image Directory"), QDir::currentPath(),
                                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    //FIX ME : add loader
    ui->textBrowser_train->append("Training images read");
    ui->textBrowser_train->setText(PARAMS.trainImagesDir);
}

void RandomDecisionForestDialogGui::onTestBrowse()
{
    PARAMS.testDir = QFileDialog::getExistingDirectory(this, tr("Open Image Directory"), PARAMS.trainImagesDir,
                                                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    //FIX ME : add loader
    ui->textBrowser_test->append("Test images read");
    ui->textBrowser_test->setText(PARAMS.testDir);
}

void RandomDecisionForestDialogGui::onTrain()
{
    m_forest.setParams(PARAMS);
    m_forest.setDataSet(*m_trainDataReaderGUI->DS());

    m_treeid = 0;
    if(m_forest.trainForest())
        ui->textBrowser_train->append("Forest Trained ! ");
    else
        ui->textBrowser_train->append("Failed to Train Forest! (Forest DS is not set) ");
    ui->textBrowser_train->repaint();
}

void RandomDecisionForestDialogGui::onTest()
{
    // FIX: fix preprocessing
    // TODO: fix preprocessing
    DataSet *DS = m_testDataReaderGUI->DS();
    int totalImgs = DS->m_ImagesVector.size();

    if (totalImgs == 0)
    {
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle("Error");
        msgBox->setText("You Should Load Test Data First!");
        msgBox->show();
        return;
    }

    float accuracy = 0;
    for (int i = 0; i < totalImgs; ++i) {
        cv::Mat img = DS->m_ImagesVector[i].clone();
        img = 255 - img; // TODO: remove when preprocess fixed
//        cv::imshow("input",img);
//        cv::waitKey();
        int label{};
        float conf{};
        m_forest.detect(img,label,conf);
        if(label == DS->m_labels[i])
            ++accuracy;
    }

    accuracy /= totalImgs;
    qDebug() << "accuracy: " << accuracy;
    //    m_forest->test(); // TODO: add test
}

void RandomDecisionForestDialogGui::new_tree_constructed()
{
    ++m_treeid;
    ui->textBrowser_train->append("Tree " + QString::number(
                                      m_treeid) + " constructed");
    ui->textBrowser_train->repaint();
}

void RandomDecisionForestDialogGui::image_at_classified_as(int index, char label)
{
    ui->textBrowser_test->append("Image " + QString::number(
                                     index) + " is classified as " + label);
    ui->textBrowser_test->repaint();
}

void RandomDecisionForestDialogGui::resultPercetange(double accuracy)
{
    std::cout << "gotcha!";
    ui->textBrowser_test->append(" Classification Accuracy : " + QString::number(
                                     accuracy) + "%");
    ui->textBrowser_test->repaint();
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
    m_forest.loadForest(fname);
    //    m_forest->printForest();
    qDebug() << "LOAD FOREST PRINTED" ;
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
    qDebug() << "SAVED FOREST PRINTED" ;
}

void RandomDecisionForestDialogGui::closeEvent(QCloseEvent *event)
{
    writeSettings();
    m_trainDataReaderGUI->writeSettings();
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

    settings.endGroup();
}

void RandomDecisionForestDialogGui::readSettings()
{
    QSettings settings("VVGLab", "RDFDialogGUI");
    settings.beginGroup("PARAMS");

    PARAMS.nTrees = settings.value("nTrees", 12).toInt();
    PARAMS.maxDepth = settings.value("maxDepth",20).toInt();
    PARAMS.probDistX = settings.value("probDistX",30).toInt();
    PARAMS.probDistY = settings.value("probDistY",30).toInt();
    PARAMS.pixelsPerImage = settings.value("pixelsPerImage",200).toInt();
    PARAMS.minLeafPixels = settings.value("minLeafPixels",5).toInt();
    PARAMS.maxIteration = settings.value("maxIteration",100).toInt();
    PARAMS.labelCount = settings.value("labelCount",26).toInt();

    settings.endGroup();
}

void RandomDecisionForestDialogGui::onNTreesChanged(int value)
{
    PARAMS.nTrees = value;
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
