#include "precompiled.h"

#include "Core/ResizeAllImagesDialogGui.h"
#include "ui_ResizeAllImagesDialogGui.h"

#include "util/Reader.h"


ResizeAllImagesDialogGui::ResizeAllImagesDialogGui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizeAllImagesDialogGui)
{
    ui->setupUi(this);
}

ResizeAllImagesDialogGui::~ResizeAllImagesDialogGui()
{
    delete ui;
}

void ResizeAllImagesDialogGui::on_pushButton_browse_clicked()
{
    m_browseDir = QFileDialog::getExistingDirectory(this,
                  tr("Open Image Directory"), QDir::currentPath(),
                  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
}

void ResizeAllImagesDialogGui::selectSaveDir()
{
    m_saveDir = QFileDialog::getExistingDirectory(this, tr("Select Save Directory"),
                QDir::currentPath(), QFileDialog::ShowDirsOnly |
                QFileDialog::DontResolveSymlinks);
}

void ResizeAllImagesDialogGui::on_pushButton_resizeall_clicked()
{
    if(m_browseDir.isEmpty())
    {
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle("Error");
        msgBox->setText("You Should First Choose Image Folder ");
        msgBox->show();
        return;
    }

    std::vector<QString> fNames;
    auto *reader = new Reader();
    reader->findImages(m_browseDir, "", fNames);
    int width = ui->spinBox_width->value();
    int height = ui->spinBox_height->value();

    for(auto filePath : fNames)
    {
        cv::Mat image = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

        if(ui->radiobutton_width->isChecked())
            width = image.cols;

        if(ui->radiobutton_height->isChecked())
            height = image.rows;

        cv::Size size(width, height);
        cv::resize(image, image, size);
        //        cv::imwrite(filePath.toStdString(), image);
    }

    qDebug() << " Resizing Done" ;
}

void ResizeAllImagesDialogGui::create()
{
    Util::convertToOSRAndBlur(m_browseDir, m_saveDir,
                               ui->spinBox_kernelSize->value());
}
