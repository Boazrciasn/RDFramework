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
    m_dataExtractMoG->setAspectConstraints(ui->aspectMin_SpinBox->value(), ui->aspectMax_SpinBox->value());
    QObject::connect(m_predictSet, SIGNAL(svmChanged()), SLOT(setSVM()));
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
    m_dataExtractMoG->setAspectConstraints(ui->aspectMin_SpinBox->value(), ui->aspectMax_SpinBox->value());
}



void DataExtractorGui::setSaveDir()
{
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Select Save Directory"), QDir::currentPath(),
                                                         QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->SaveDir_label->setText(fileName);
    m_dataExtractMoG->setSaveDir(fileName);
}

void DataExtractorGui::setSVM()
{
    m_hogSVM.setSVM(m_predictSet->getSvm());
    ui->classificationInfo_text->setText(m_predictSet->svmFile());
    ui->setup_label->setStyleSheet("background-color: green");
}

