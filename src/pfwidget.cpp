#include "pfwidget.h"
#include "ui_pfwidget.h"

PFWidget::PFWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PFWidget)
{
    ui->setupUi(this);
}

PFWidget::~PFWidget()
{
    delete ui;
}
