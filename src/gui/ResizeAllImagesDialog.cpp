#include "src/util/precompiled.h"

#include "ResizeAllImagesDialog.h"
#include "ui_ResizeallImagesDialog.h"

#include "Reader.h"


ResizeAllImagesDialog::ResizeAllImagesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizeAllImagesDialog)
{
    ui->setupUi(this);
}

ResizeAllImagesDialog::~ResizeAllImagesDialog()
{
    delete ui;
}

void ResizeAllImagesDialog::on_pushButton_browse_clicked()
{
    m_browseDir = QFileDialog::getExistingDirectory(this,
                  tr("Open Image Directory"), QDir::currentPath(),
                  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
}

void ResizeAllImagesDialog::selectSaveDir()
{
    m_saveDir = QFileDialog::getExistingDirectory(this, tr("Select Save Directory"),
                QDir::currentPath(), QFileDialog::ShowDirsOnly |
                QFileDialog::DontResolveSymlinks);
}

void ResizeAllImagesDialog::on_pushButton_resizeall_clicked()
{
    if(m_browseDir == NULL)
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

void ResizeAllImagesDialog::create()
{
    Util::convertToOSRAndBlure(m_browseDir, m_saveDir,
                               ui->spinBox_kernelSize->value());
}
