#include "precompiled.h"

#include "include/resizeallimagesdialog.h"
#include "ui_resizeallimagesdialog.h"

#include "include/Reader.h"


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
    dir = QFileDialog::getExistingDirectory(this,tr("Open Image Direrctory"), QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
}

void ResizeAllImagesDialog::on_pushButton_resizeall_clicked()
{
    if(dir == NULL)
    {
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle("Error");
        msgBox->setText("You Should First Choose Image Folder ");
        msgBox->show();
        return;
    }

    std::vector<QString> fNames;
    auto *reader = new Reader();
    reader->findImages(dir, "", fNames);

    int width = ui->spinBox_width->value();
    int height = ui->spinBox_height->value();
    cv::Size size(width,height);

    for(auto filePath : fNames)
    {
        cv::Mat image = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
        cv::resize(image,image,size);
        cv::imwrite(filePath.toStdString(), image);
    }
}
