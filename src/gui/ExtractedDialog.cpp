#include "src/util/precompiled.h"

#include "ExtractedDialog.h"
#include "ui_dialogextracted.h"

DialogExtracted::DialogExtracted(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExtracted)
{
    ui->setupUi(this);
    ui->label->setScaledContents(true);
}

void DialogExtracted::setLabel(QImage &image)
{
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->label->repaint();
}

DialogExtracted::~DialogExtracted()
{
    delete ui;
}
