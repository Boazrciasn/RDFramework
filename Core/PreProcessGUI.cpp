#include "PreProcessGUI.h"
#include "ui_PreProcessGUI.h"

PreProcessGUI::PreProcessGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreProcessGUI)
{
    ui->setupUi(this);
}

PreProcessGUI::~PreProcessGUI()
{
    delete ui;
}
