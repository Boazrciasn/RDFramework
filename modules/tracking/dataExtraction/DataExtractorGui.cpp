#include "precompiled.h"

#include "DataExtractor.h"
#include "DataExtractorGui.h"
#include "ui_DataExtractorGui.h"
#include "BackgroundSubtractors.h"

DataExtractorGui::DataExtractorGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataExtractorGui)
{
    m_bgsubMoG.setParam(5, 5);
    m_dataExtractMoG = new DataExtractor<BackgroundSubMoG, int>(m_bgsubMoG, 10);
    ui->setupUi(this);
    m_testWin = new VideoPlayerGui(this);
    m_predictSet = new PredictorGui(this);
    ui->main_gridLayout->addWidget(m_testWin, 0, 0);
    ui->Settings_groupBox_layout->addWidget(m_predictSet);
    m_testWin->setProcess(m_dataExtractMoG);
    m_dataExtractMoG->setBBoxSizeConstraints(ui->min_spinBox->value(), ui->max_spinBox->value());
    ui->classificationInfo_text->setText("Default");

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

void DataExtractorGui::spinBoxesUpdate()
{
    m_dataExtractMoG->setBBoxSizeConstraints(ui->min_spinBox->value(), ui->max_spinBox->value());
}
