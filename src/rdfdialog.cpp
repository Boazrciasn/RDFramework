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
    m_train_dir = QFileDialog::getExistingDirectory(this,tr("Open Image Directory"), QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->textBrowser_train->setText(m_train_dir);
}

void RDFDialog::on_loadTestData_button_clicked()
{
    m_test_dir = QFileDialog::getExistingDirectory(this,tr("Open Image Directory"), m_train_dir,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->textBrowser_test->setText(m_test_dir);
}

void RDFDialog::on_train_button_clicked()
{
    if(m_train_dir == NULL)
    {
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle("Error");
        msgBox->setText("You Should First Choose a Training Data Folder");
        msgBox->show();
        return;
    }

    m_treeid = 0;

    RDFParams PARAMS;
    PARAMS.trainDir = m_train_dir;
    PARAMS.probDistX = ui->spinBox_probX->value();
    PARAMS.probDistY = ui->spinBox_probY->value();
    PARAMS.nTrees = ui->spinBox_NTrees->value();
    PARAMS.maxDepth = ui->spinBox_MaxDepth->value();
    PARAMS.pixelsPerImage = ui->spinBox_PixelsPerImage->value();
    PARAMS.minLeafPixels = ui->spinBox_MinLeafPixels->value();
    PARAMS.labelCount = ui->spinBox_LabelCount->value();
    PARAMS.maxIteration = ui->spinBox_MaxIteration->value();

    m_forest->setParams(PARAMS);

    ui->textBrowser_train->append("Training images read");
    ui->textBrowser_train->repaint();

    m_forest->trainForest();
    ui->textBrowser_train->append(  "Forest Trained ! ");
    ui->textBrowser_train->repaint();
}

void RDFDialog::on_test_button_clicked()
{

    if(m_test_dir == NULL)
    {
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle("Error");
        msgBox->setText("You Should First Choose a Test Data Folder");
        msgBox->show();
        return;
    }

//    if(m_forest->tempTree.size()==0)
//    {
//        QMessageBox* msgBox = new QMessageBox();
//        msgBox->setWindowTitle("Error");
//        msgBox->setText("You Should First Train the Forest");
//        msgBox->show();
//        return;
//    }

    m_forest->setTestPath(m_test_dir);
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
