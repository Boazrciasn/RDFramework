#include "DataExtractorGui.h"
#include "ui_DataExtractorGui.h"

DataExtractorGui::DataExtractorGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataExtractorGui)
{
    ui->setupUi(this);
    m_testWin = new VideoPlayerGui(this);
    ui->main_gridLayout->addWidget(m_testWin);
}

DataExtractorGui::~DataExtractorGui()
{
    delete ui;
}

void DataExtractorGui::createNewWindow()
{
    if (m_testWin->isVisible())
        m_testWin->hide();
    else
        m_testWin->show();
}
