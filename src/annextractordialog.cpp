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

//    QPixmap OriginalPix(*ui->imageLabel->pixmap());

    QImage newImage;
    newImage = OriginalPix.toImage();
    this->copyImage = newImage.copy(myRect);

//    ui->imageLabel->setPixmap(QPixmap::fromImage(copyImage));
//    ui->imageLabel->repaint();

    DialogExtracted *mDialog = new DialogExtracted(this);
    mDialog->setLabel(this->copyImage);
    mDialog->exec();
}


void AnnExtractorDialog::on_browse_button_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
    QString searchChar = ui->search_char->text();

    if (searchChar.size() != 1){
        QMessageBox::information(this, tr("Image Viewer"),
                                 tr("Please check your input parameters!"));
        return;
    }


    if (!fileName.isEmpty()) {
        QImage image(fileName);

        if (image.isNull()) {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(fileName));
            return;
        }

        ui->imageLabel->setPixmap(QPixmap::fromImage(image));
        ui->imageLabel->repaint();
    }
}

void AnnExtractorDialog::on_save_button_clicked()
{
    rubberBand->hide();

    if(!this->copyImage.save("./test.jpg"))
        QMessageBox::information(this, tr("SAVE FAILED"),
                                 tr("Unable to save."));
}

void AnnExtractorDialog::on_previous_button_clicked()
{

}

void AnnExtractorDialog::on_next_button_clicked()
{

}
