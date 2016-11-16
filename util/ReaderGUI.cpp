#include "ReaderGUI.h"
#include "ui_ReaderGUI.h"

ReaderGUI::ReaderGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReaderGUI)
{
    ui->setupUi(this);
    m_reader = new Reader();
    m_DS = new DataSet();
    m_dFlag = ui->DatasetType_comboBox->currentIndex();
}

ReaderGUI::~ReaderGUI()
{
    delete ui;
}


void ReaderGUI::load()
{
    switch (m_dFlag)
    {
    case Type_Standard:
    {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Image Directory"), QDir::currentPath(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!m_DS->m_testImagesVector.empty())
        {
            ui->LoadedDataset_label->setText(dir);
            emit imagesLoaded(true);
        }
        break;
    }
    case Type_MNIST:
    {
        QString dir = QFileDialog::getOpenFileName(this, tr("Open Dataset File"), QDir::currentPath());
        if (!dir.isEmpty())
        {
            m_reader->readImages(dir, m_DS->m_testImagesVector, m_dFlag);
            if (!m_DS->m_testImagesVector.empty())
            {
                ui->LoadedDataset_label->setText(dir);
                emit imagesLoaded(true);
            }
            dir.replace(QRegExp("images.idx3"), "labels.idx1");
            m_reader->readLabels(dir, m_DS->m_testlabels, m_dFlag);
            if (!m_DS->m_testlabels.empty())
            {
                ui->labelFilepath_label->setText(dir);
                emit labelsLoaded(true);
            }
        }
        break;
    }
    default:
        break;
    }
}

void ReaderGUI::dataTypeChanged(int index)
{
    if(index == Type_MNIST)
        ui->loadLabels_checkBox->hide();
    else
        ui->loadLabels_checkBox->show();

    m_dFlag = index;
}
