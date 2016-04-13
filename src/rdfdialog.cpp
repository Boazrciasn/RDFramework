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
    train_dir = QFileDialog::getExistingDirectory(this,tr("Open Image Direrctory"), QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->label_train->setText(train_dir);
}

void RDFDialog::on_loadTestData_button_clicked()
{
    test_dir = QFileDialog::getExistingDirectory(this,tr("Open Image Direrctory"), QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->label_test->setText(test_dir);
}

void RDFDialog::on_train_button_clicked()
{
    int probDistX = ui->spinBox_probX->value();
    int probDistY = ui->spinBox_probY->value();
    int nTrees = ui->spinBox_NTrees->value();
    int maxDepth = ui->spinBox_MaxDepth->value();

    forest = new RandomDecisionForest(probDistX,probDistY);
    forest->setTrainPath(train_dir);
    forest->setNumberofTrees(nTrees);
    forest->readTrainingImageFiles();
    forest->trainForest();
    ui->label_train->setText( ui->label_train->text() +  "\n Training images read");
}

void RDFDialog::on_test_button_clicked()
{
    forest->setTestPath(test_dir);
    forest->readTestImageFiles();
    ui->label_test->setText(ui->label_test->text() +  "\n Test images read");
    forest->test();
}
