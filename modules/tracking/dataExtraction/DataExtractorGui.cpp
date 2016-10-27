#include "precompiled.h"

#include "DataExtractor.h"
#include "DataExtractorGui.h"
#include "ui_DataExtractorGui.h"


DataExtractorGui::DataExtractorGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataExtractorGui)
{
    ui->setupUi(this);
    m_bgsubMoG.setParam(5, 5);
    m_hogSVM.init();
    m_dataExtractMoG = new DataExtractor<BackgroundSubMoG, HoGSVM>(m_bgsubMoG, m_hogSVM);
    m_player = new VideoPlayerGui(this);
    m_predictSet = new PredictorGui(this);
    ui->main_gridLayout->addWidget(m_player, 0, 0);
    ui->Settings_groupBox_layout->addWidget(m_predictSet);
    m_player->setProcess(m_dataExtractMoG);
    m_dataExtractMoG->setBBoxSizeConstraints(ui->min_spinBox->value(), ui->max_spinBox->value());


}

DataExtractorGui::~DataExtractorGui()
{
    delete ui;
}

void DataExtractorGui::createNewWindow()
{
    if (m_player->isVisible())
        m_player->hide();
    else
        m_player->show();
}

void DataExtractorGui::spinBoxesUpdate()
{
    m_dataExtractMoG->setBBoxSizeConstraints(ui->min_spinBox->value(), ui->max_spinBox->value());
}

void DataExtractorGui::setupExtractor()
{
    m_hogSVM.setSVM(m_predictSet->getSvm());
    ui->classificationInfo_text->setText(m_predictSet->svmFile());
    ui->setup_pushButton->setStyleSheet("background-color: green");

}
