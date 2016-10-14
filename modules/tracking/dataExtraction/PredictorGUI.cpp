#include "PredictorGUI.h"
#include "ui_PredictorGUI.h"

PredictorGUI::PredictorGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PredictorGUI)
{
    ui->setupUi(this);
}

PredictorGUI::~PredictorGUI()
{
    delete ui;
}
