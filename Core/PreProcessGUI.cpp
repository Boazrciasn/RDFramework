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

void PreProcessGUI::onKernelChanged()
{
    auto val = ui->spinBox_Sigma->value();
    if (val % 2 == 0)
    {
        QMessageBox b;
        b.setText("Only odd values allowed!");
        b.exec();
        ui->spinBox_Sigma->setValue(val - 1);
    }
}

void PreProcessGUI::createPreProcesses()
{
    m_processes.clear();
    if (ui->checkBox_InverseImg->isChecked())
        m_processes.push_back(new InverseImage());

    if (ui->groupBox_Sobel->isChecked())
    {
        Sobel* proc = new Sobel(ui->spinBox_KernelX_Sobel->value(), ui->spinBox_KernelY_Sobel->value(),1);
        if(ui->checkBox_Schar)
            proc->setKSize(CV_SCHARR);
        m_processes.push_back(proc);
    }

    if (ui->groupBox_Gaussian_Blur->isChecked())
        m_processes.push_back(new Gaussian(ui->spinBox_KernelX->value(), ui->spinBox_KernelY->value(),
                                           ui->spinBox_Sigma->value()));
}

