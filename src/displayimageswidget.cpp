#include "precompiled.h"

#include "include/displayimageswidget.h"
#include "ui_displayimageswidget.h"

DisplayImagesWidget::DisplayImagesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayImagesWidget)
{
    ui->setupUi(this);
    m_reader = new Reader();
}

DisplayImagesWidget::~DisplayImagesWidget()
{
    delete ui;
}

void DisplayImagesWidget::display()
{

}

void DisplayImagesWidget::browseButton_clicked()
{
    m_dir = QFileDialog::getExistingDirectory(this, tr("Open Image Direrctory"),
                                            "/home/vvglab/Desktop/ImageCLEF2016/pages_devel"/*QDir::currentPath()*/,
                                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_fileIndex = 0;
    m_reader->readFromTo(m_dir.toStdString(), m_fNames);
    if (!m_fNames.empty())
    {
        display();
        //        mDialog = new MyDialog(this);
        //        mDialog->show();
        //        mDialog->setFNames(this->fNames,dir);
    }
}

void DisplayImagesWidget::prevButton_clicked()
{
    m_fileIndex--;
    if (m_fileIndex < 0)
        m_fileIndex++;
    display();
}

void DisplayImagesWidget::nextButton_clicked()
{
    m_fileIndex++;
    if (m_fileIndex >= (int)m_fNames.size())
        m_fileIndex--;
    display();
}
