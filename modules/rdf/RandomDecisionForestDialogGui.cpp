#include "precompiled.h"

#include "rdf/RDFParams.h"
#include "rdf/RandomDecisionForestDialogGui.h"
#include "ui_RandomDecisionForestDialogGui.h"


RandomDecisionForestDialogGui::RandomDecisionForestDialogGui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RandomDecisionForestDialogGui)
{
    ui->setupUi(this);
//    QObject::connect(m_forest.get(), SIGNAL(treeConstructed()), this,
//                     SLOT(new_tree_constructed()));
//    QObject::connect(m_forest.get(), SIGNAL(classifiedImageAs(int, char)), this,
//                     SLOT(image_at_classified_as(int, char)));
//    QObject::connect(m_forest.get(), SIGNAL(resultPercentage(double)), this,
//                     SLOT(resultPercetange(double))) ;
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
    PARAMS.probDistX = ui->spinBox_probX->value();
    PARAMS.probDistY = ui->spinBox_probY->value();
    PARAMS.nTrees = ui->spinBox_NTrees->value();
    PARAMS.maxDepth = ui->spinBox_MaxDepth->value();
    PARAMS.pixelsPerImage = ui->spinBox_PixelsPerImage->value();
    PARAMS.minLeafPixels = ui->spinBox_MinLeafPixels->value();
    PARAMS.labelCount = ui->spinBox_LabelCount->value();
    PARAMS.maxIteration = ui->spinBox_MaxIteration->value();
    m_forest.setParams(PARAMS);
    if (m_forest.params().trainImagesDir.isEmpty())
    {
        auto *msgBox = new QMessageBox();
        msgBox->setWindowTitle("Error");
        msgBox->setText("You Should First Choose a Training Data Folder");
        msgBox->show();
        return;
    }
    Util::calcWidthHeightStat(m_forest.params().trainImagesDir);
    m_treeid = 0;
    m_forest.trainForest();
    ui->textBrowser_train->append("Forest Trained ! ");
    ui->textBrowser_train->repaint();
}

void RandomDecisionForestDialogGui::onTest()
{
    m_forest.params().testDir = PARAMS.testDir;
    //m_forest->readAndIdentifyWords();
    if (m_forest.params().testDir.isEmpty())
    {
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle("Error");
        msgBox->setText("You Should First Choose a Test Data Folder");
        msgBox->show();
        return;
    }
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
    //QString fname = QFileDialog::getExistingDirectory(this,tr("Load Forest Directory"), QDir::currentPath(), QFileDialog::AnyFile);
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
