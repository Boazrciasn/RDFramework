#include "Core/DisplayGUI.h"
#include "ui_DisplayGUI.h"

DisplayGUI::DisplayGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayGUI)
{
    ui->setupUi(this);
}

DisplayGUI::~DisplayGUI()
{
    delete ui;
}
