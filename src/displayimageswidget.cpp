#include "displayimageswidget.h"
#include "ui_displayimageswidget.h"

DisplayImagesWidget::DisplayImagesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayImagesWidget)
{
    ui->setupUi(this);
}

DisplayImagesWidget::~DisplayImagesWidget()
{
    delete ui;
}
