#include "precompiled.h"

#include "ocr/ExtractedDialogGui.h"
#include "ui_ExtractedDialogGui.h"

DialogExtractedGui::DialogExtractedGui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExtractedGui)
{
    ui->setupUi(this);
    ui->label->setScaledContents(true);
}

void DialogExtractedGui::setLabel(QImage &image)
{
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->label->repaint();
}

DialogExtractedGui::~DialogExtractedGui()
{
    delete ui;
}
