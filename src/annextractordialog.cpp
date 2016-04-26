#include "precompiled.h"

#include "include/annextractordialog.h"
#include "ui_annextractordialog.h"

AnnExtractorDialog::AnnExtractorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnExtractorDialog)
{
    ui->setupUi(this);
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    ui->imageLabel->setScaledContents(true);
}

AnnExtractorDialog::~AnnExtractorDialog()
{
    delete ui;
}

void AnnExtractorDialog::mouseMoveEvent(QMouseEvent *event)
{
    rubberBand->setGeometry(QRect(myPoint, event->pos()).normalized());
}

void AnnExtractorDialog::mousePressEvent(QMouseEvent *event)
{
    if (rubberBand->isEnabled())
        rubberBand->hide();

    if(ui->imageLabel->underMouse()){
        myPoint = event->pos();
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->show();
    }
}

void AnnExtractorDialog::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint a = mapToGlobal(myPoint);
    QPoint b = event->globalPos();

    a = ui->imageLabel->mapFromGlobal(a);
    b = ui->imageLabel->mapFromGlobal(b);
    QPixmap OriginalPix(*ui->imageLabel->pixmap());
    double sx = ui->imageLabel->rect().width();
    double sy = ui->imageLabel->rect().height();
    sx = OriginalPix.width() / sx;
    sy = OriginalPix.height() / sy;

    a = QPoint(int(a.x() * sx),int(a.y() * sy));
    b = QPoint(int(b.x() * sx),int(b.y() * sy));

    QRect myRect(a, b);

    ui->width_height_label->setText(" W: " + QString::number(myRect.width()) +
                                    " H: " + QString::number(myRect.height()));

//    QPixmap OriginalPix(*ui->imageLabel->pixmap());

    QImage newImage;
    newImage = OriginalPix.toImage();
    this->copyImage = newImage.copy(myRect);
    ui->preview_label->setPixmap(QPixmap::fromImage(this->copyImage));
    ui->preview_label->repaint();
//    ui->imageLabel->setPixmap(QPixmap::fromImage(copyImage));
//    ui->imageLabel->repaint();

//    DialogExtracted *mDialog = new DialogExtracted(this);
//    mDialog->setLabel(this->copyImage);
//    mDialog->exec();
}


void AnnExtractorDialog::on_browse_button_clicked()
{
    QString searchChar = ui->search_char->text();

    if (searchChar.size() != 1){
        QMessageBox::information(this, tr("Image Viewer"),
                                 tr("Please check your input parameters!"));
        return;
    }

    dir = QFileDialog::getExistingDirectory(this,tr("Open Image Direrctory"), QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    fNames.clear();
    reader.findImages(dir,searchChar,fNames);

    if(fNames.size() == 0)
    {
        QMessageBox::information(this, tr("Reader"),
                                 tr("loading failed!"));
        return;
    }

    // Add save dir
    int posLastSlash = dir.lastIndexOf("/",-1);
    this->saveDir = dir.mid(0,posLastSlash) + "/AnnotationResults/" + ui->search_char->text();

    QFile textFileRead(this->saveDir + "/lastSession.txt");
    if (textFileRead.exists()) {
        textFileRead.open(QIODevice::ReadOnly);
        QString str = textFileRead.readLine();
        QStringList strList = str.split("#");
        savedFileCounter = ((QString)strList[0]).toInt();
        fileIndex = ((QString)strList[1]).toInt() + 1;

        ui->counter_label->setText(QString::number(savedFileCounter));

        qDebug() << savedFileCounter << " - " << fileIndex;
    }else {
        fileIndex = 0;
        savedFileCounter = 0;
    }

    QImage image(fNames[fileIndex]);
    ui->imageLabel->setPixmap(QPixmap::fromImage(image));
    QPixmap pixmap = QPixmap::fromImage(image);
    QImage scaledImage = pixmap.toImage().scaled(pixmap.size() * devicePixelRatio(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(devicePixelRatio());
    QPixmap* newScaledPixmap = new QPixmap(QPixmap::fromImage(scaledImage));
    ui->imageLabel->setScaledContents(true);
    ui->imageLabel->resize(ui->imageLabel->pixmap()->size());
    ui->imageLabel->setPixmap(*newScaledPixmap);
    QString fname =  fNames[fileIndex];
    posLastSlash = fname.lastIndexOf("/",-1);
    int posPrevSlash = fname.lastIndexOf("/",-(fname.length() - posLastSlash +1));

    ui->out_info_label->setText(fname.mid(posPrevSlash+1,posLastSlash-posPrevSlash-1));
    ui->imageLabel->repaint();


    QDir dir_save(saveDir);
    if (!dir_save.exists())
    {
        dir_save.mkpath(".");
        if(!dir_save.exists())
             qDebug() << "ERROR : " << dir_save << " can not be created!" ;
    }

//    savedFileCounter = 0;   // initial value


//    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
//    QString searchChar = ui->search_char->text();

//    if (searchChar.size() != 1){
//        QMessageBox::information(this, tr("Image Viewer"),
//                                 tr("Please check your input parameters!"));
//        return;
//    }


//    if (!fileName.isEmpty()) {
//        QImage image(fileName);

//        if (image.isNull()) {
//            QMessageBox::information(this, tr("Image Viewer"),
//                                     tr("Cannot load %1.").arg(fileName));
//            return;
//        }

//        ui->imageLabel->setPixmap(QPixmap::fromImage(image));
//        ui->imageLabel->repaint();
//    }
}

void AnnExtractorDialog::on_save_button_clicked()
{
    rubberBand->hide();

    QString file = this->saveDir + "/" + ui->search_char->text() + "_" + QString::number(savedFileCounter) + ".jpg";
    if(!this->copyImage.save(file))
        QMessageBox::information(this, tr("SAVE FAILED"),
                                 tr("Unable to save."));
    savedFileCounter++;
    ui->counter_label->setText(QString::number(savedFileCounter));

    QFile textFile(this->saveDir + "/lastSession.txt");
    textFile.open(QIODevice::WriteOnly);
    textFile.write(QByteArray::number(savedFileCounter));
    textFile.write("#");
    textFile.write(QByteArray::number(fileIndex));
    textFile.close();
}

void AnnExtractorDialog::on_previous_button_clicked()
{
    fileIndex--;
    if (fileIndex < 0)
    {
        fileIndex++;
        return;
    }
    QImage image(fNames[fileIndex]);
    QPixmap pixmap = QPixmap::fromImage(image);
    QImage scaledImage = pixmap.toImage().scaled(pixmap.size() * devicePixelRatio(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(devicePixelRatio());
    QPixmap* newScaledPixmap = new QPixmap(QPixmap::fromImage(scaledImage));
    ui->imageLabel->setScaledContents(true);
    ui->imageLabel->resize(ui->imageLabel->pixmap()->size());
    ui->imageLabel->setPixmap(*newScaledPixmap);

    QString fname =  fNames[fileIndex];
    int posLastSlash = fname.lastIndexOf("/",-1);
    int posPrevSlash = fname.lastIndexOf("/",-(fname.length() - posLastSlash +1));

    ui->out_info_label->setText(fname.mid(posPrevSlash+1,posLastSlash-posPrevSlash-1));
    ui->imageLabel->repaint();
}

void AnnExtractorDialog::on_next_button_clicked()
{
    fileIndex++;
    if (fileIndex >= (int)this->fNames.size())
    {
        fileIndex--;
        return;
    }
    QImage image(fNames[fileIndex]);


    QPixmap pixmap = QPixmap::fromImage(image);
    QImage scaledImage = pixmap.toImage().scaled(pixmap.size() * devicePixelRatio(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(devicePixelRatio());
    QPixmap* newScaledPixmap = new QPixmap(QPixmap::fromImage(scaledImage));
    ui->imageLabel->setScaledContents(true);
    ui->imageLabel->resize(ui->imageLabel->pixmap()->size());
    ui->imageLabel->setPixmap(*newScaledPixmap);
    QString fname =  fNames[fileIndex];
    int posLastSlash = fname.lastIndexOf("/",-1);
    int posPrevSlash = fname.lastIndexOf("/",-(fname.length() - posLastSlash +1));

    ui->out_info_label->setText(fname.mid(posPrevSlash+1,posLastSlash-posPrevSlash-1));
    ui->imageLabel->repaint();
}
