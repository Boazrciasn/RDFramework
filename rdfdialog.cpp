#include "rdfdialog.h"
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
