#include "precompiled.h"

#include "include/RDFParams.h"
#include "include/rdfdialog.h"
#include "ui_rdfdialog.h"


RDFDialog::RDFDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RDFDialog)
{
    ui->setupUi(this);
    m_forest = new RandomDecisionForest();
    QObject::connect(m_forest, SIGNAL(treeConstructed()), this, SLOT(new_tree_constructed()));
    QObject::connect(m_forest, SIGNAL(classifiedImageAs(int,char)), this, SLOT(image_at_classified_as(int,char)));
}

RDFDialog::~RDFDialog()
{
    delete ui;
}

void RDFDialog::on_loadTrainData_button_clicked()
{
    PARAMS.trainDir = QFileDialog::getExistingDirectory(this,tr("Open Image Directory"), QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->textBrowser_train->setText(PARAMS.trainDir);
}

void RDFDialog::on_loadTestData_button_clicked()
{
    PARAMS.testDir = QFileDialog::getExistingDirectory(this,tr("Open Image Directory"), PARAMS.trainDir,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->textBrowser_test->setText(PARAMS.testDir);
}

void RDFDialog::on_train_button_clicked()
{

    PARAMS.probDistX = ui->spinBox_probX->value();
    PARAMS.probDistY = ui->spinBox_probY->value();
    PARAMS.nTrees = ui->spinBox_NTrees->value();
    PARAMS.maxDepth = ui->spinBox_MaxDepth->value();
    PARAMS.pixelsPerImage = ui->spinBox_PixelsPerImage->value();
    PARAMS.minLeafPixels = ui->spinBox_MinLeafPixels->value();
    PARAMS.labelCount = ui->spinBox_LabelCount->value();
    PARAMS.maxIteration = ui->spinBox_MaxIteration->value();

    m_forest->setParams(PARAMS);


    if(m_forest->params().trainDir == NULL)
    {
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle("Error");
        msgBox->setText("You Should First Choose a Training Data Folder");
        msgBox->show();
        return;
    }

    Util::calcWidthHightStat(m_forest->params().trainDir);
    m_treeid = 0;
    m_forest->readTrainingImageFiles();
    ui->textBrowser_train->append("Training images read");
    ui->textBrowser_train->repaint();

    m_forest->trainForest();
    ui->textBrowser_train->append(  "Forest Trained ! ");
    ui->textBrowser_train->repaint();
}

void RDFDialog::on_test_button_clicked()
{

    m_forest->params().testDir = PARAMS.testDir;

    if(m_forest->params().testDir == NULL)
    {
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle("Error");
        msgBox->setText("You Should First Choose a Test Data Folder");
        msgBox->show();
        return;
    }


    m_forest->readTestImageFiles();
    ui->textBrowser_test->append("Test images read");
    m_forest->test();
}

void RDFDialog::new_tree_constructed()
{
    ++m_treeid;
    ui->textBrowser_train->append("Tree " + QString::number(m_treeid) + " constructed");
    ui->textBrowser_train->repaint();
}

void RDFDialog::image_at_classified_as(int index, char label)
{
    ui->textBrowser_test->append("Image " + QString::number(index) + " is classified as " + label);
    ui->textBrowser_test->repaint();
}
