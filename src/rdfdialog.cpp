#include "include/rdfdialog.h"
#include "ui_rdfdialog.h"

RDFDialog::RDFDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RDFDialog)
{
    ui->setupUi(this);

}

RDFDialog::~RDFDialog()
{
    delete ui;
}

void RDFDialog::on_loadTrainData_button_clicked()
{
    train_dir = QFileDialog::getExistingDirectory(this,tr("Open Image Directory"), QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->label_train->setText(train_dir);
}

void RDFDialog::on_loadTestData_button_clicked()
{
    test_dir = QFileDialog::getExistingDirectory(this,tr("Open Image Directory"), train_dir,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->label_test->setText(test_dir);
}

void RDFDialog::on_train_button_clicked()
{
    int probDistX = ui->spinBox_probX->value();
    int probDistY = ui->spinBox_probY->value();
    int nTrees = ui->spinBox_NTrees->value();
    int maxDepth = ui->spinBox_MaxDepth->value();
    int pixelsperImage = ui->spinBox_PixelsPerImage->value();
    int minLeafPixels = ui->spinBox_MinLeafPixels->value();
    int labelCount = ui->spinBox_LabelCount->value();
    int maxIteration = ui->spinBox_MaxIteration->value();



    forest = new RandomDecisionForest(probDistX,probDistY);
    forest->setTrainPath(train_dir);
    forest->setNumberofTrees(nTrees);
    forest->setMaxDepth(maxDepth);
    forest->setPixelsPerImage(pixelsperImage);
    forest->setMinimumLeafPixelCount(minLeafPixels);
    forest->setNumberofLabels(labelCount);
    forest->setMaxIterationForDivision(maxIteration);
    forest->readTrainingImageFiles();
    ui->label_train->setText( ui->label_train->text() +  "\n Training images read");
    forest->trainForest();
    ui->label_train->setText(  "Forest Trained ! ");
}

void RDFDialog::on_test_button_clicked()
{
    forest->setTestPath(test_dir);
    forest->readTestImageFiles();
    ui->label_test->setText(ui->label_test->text() +  "\n Test images read");
    forest->test();
}
