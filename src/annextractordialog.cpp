#include "precompiled.h"

#include "include/annextractordialog.h"
#include "ui_annextractordialog.h"
#include "include/Util.h"

AnnExtractorDialog::AnnExtractorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnExtractorDialog)
{
    ui->setupUi(this);
    m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    ui->imageLabel->setScaledContents(true);
    m_sampleCount = 0;
}

AnnExtractorDialog::~AnnExtractorDialog()
{
    delete ui;
    writeHeader();
}

void AnnExtractorDialog::mouseMoveEvent(QMouseEvent *event)
{
    m_rubberBand->setGeometry(QRect(m_Point, event->pos()).normalized());
}

void AnnExtractorDialog::mousePressEvent(QMouseEvent *event)
{
    if (m_rubberBand->isEnabled())
        m_rubberBand->hide();

    if(ui->imageLabel->underMouse())
    {
        m_Point = event->pos();
        m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        m_rubberBand->show();
    }
}

void AnnExtractorDialog::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint a = mapToGlobal(m_Point);
    QPoint b = event->globalPos();
    a = ui->imageLabel->mapFromGlobal(a);
    b = ui->imageLabel->mapFromGlobal(b);
    QPixmap OriginalPix(*ui->imageLabel->pixmap());
    double sx = ui->imageLabel->rect().width();
    double sy = ui->imageLabel->rect().height();
    sx = OriginalPix.width() / sx;
    sy = OriginalPix.height() / sy;
    a = QPoint(int(a.x() * sx), int(a.y() * sy));
    b = QPoint(int(b.x() * sx), int(b.y() * sy));
    m_rect = QRect(a, b);
    ui->width_height_label->setText(" W: " + QString::number(m_rect.width()) +
                                    " H: " + QString::number(m_rect.height()));
    //    QPixmap OriginalPix(*ui->imageLabel->pixmap());
    QImage newImage;
    newImage = OriginalPix.toImage();
    this->copyImage = newImage.copy(m_rect);
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
    if(m_sampleCount != 0)
    {
        if (m_searchChar != ui->search_char->text())
        {
            int ret  = QMessageBox::warning(this, tr("Annotation Extraction"),
                                            tr(("You are swtiching between LABELS! \n Do you want to save extracted " +
                                                m_searchChar + " label samples?").toStdString().c_str()),
                                            QMessageBox::Save | QMessageBox::Cancel, QMessageBox::Save);

            switch (ret)
            {
            case QMessageBox::Save:
                writeHeader();
                qDebug() << "SAVED";
                m_searchChar = ui->search_char->text();

                if(!isBrowseble() || !loadFiles())
                    return;

                createSaveDir();
                createSaveFile();
                display();
                break;

            case QMessageBox::Cancel:
            default:
                qDebug() << "GFY SIR or MADAM!";
                ui->search_char->text() = m_searchChar;
                break;
            }
        }

        else
        {
            return;
        }
    }

    else
    {
        m_searchChar = ui->search_char->text();

        if(!isBrowseble() || !loadFiles())
            return;

        createSaveDir();
        createSaveFile();
        display();
    }
}

//TODO unnecessarry check
bool AnnExtractorDialog::isBrowseble()
{
    if (m_searchChar.size() != 1)
    {
        QMessageBox::information(this, tr("Image Viewer"),
                                 tr("Please check your input parameters!"));
        return false;
    }

    return true;
}

bool AnnExtractorDialog::loadFiles()
{
    m_Loaddir = QFileDialog::getExistingDirectory(this, tr("Open Image Directory"),
                QDir::currentPath(), QFileDialog::ShowDirsOnly |
                QFileDialog::DontResolveSymlinks);
    m_fNames.clear();
    m_reader.findImages(m_Loaddir, m_searchChar, m_fNames);

    if(m_fNames.size() == 0)
    {
        QMessageBox::information(this, tr("Reader"),
                                 tr("loading failed!"));
        return false;
    }

    return true;
}

void AnnExtractorDialog::createSaveDir()
{
    int posLastSlash = m_Loaddir.lastIndexOf("/", -1);
    m_saveDir = m_Loaddir.mid(0,
                              posLastSlash) + "/AnnotationResults2/" + m_searchChar;
    QDir dir_save(m_saveDir);

    if (!dir_save.exists())
    {
        dir_save.mkpath(".");

        if(!dir_save.exists())
            qDebug() << "ERROR : " << dir_save << " can not be created!" ;
    }
}

void AnnExtractorDialog::createSaveFile()
{
    m_saveFile = new QFile(m_saveDir + "/" + m_searchChar + "_lastSession.txt");
    bool fileExists = m_saveFile->exists();

    if (fileExists)
    {
        loadHeader();
    }

    else
    {
        // set initial values and create header
        m_avgWidth = 0;
        m_avgHight = 0;
        m_fileIndex = 0;
        m_sampleCount = 0;
        writeHeader();
    }
}

void AnnExtractorDialog::loadHeader()
{
    m_saveFile->open(QIODevice::ReadWrite);
    int count = 0;
    QString str;

    do
    {
        str =  m_saveFile->readLine();
        count++;

        switch (count)
        {
        case 1:
            m_avgWidth = str.split(" ")[0].toInt();
            break;

        case 2:
            m_avgHight = str.split(" ")[0].toInt();
            break;

        case 3:
            m_sampleCount = str.split(" ")[0].toInt();
            break;

        case 4:
            m_fileIndex = str.split(" ")[0].toInt();
            break;

        default:
            std::cout << str.split(" ")[0].toStdString() << std::endl;
            break;
        }
    }
    while(m_saveFile->canReadLine() && (count < FILE_HEADER));

    ui->counter_label->setText(QString::number(m_sampleCount));
    m_saveFile->flush();
    m_saveFile->close();
}

void AnnExtractorDialog::writeHeader()
{
    m_saveFile->open(QIODevice::ReadWrite | QIODevice::Text);
    QString s;
    QTextStream t(m_saveFile);
    s += QString::number(m_avgWidth) + "\n";
    s += QString::number(m_avgHight) + "\n";
    s += QString::number(m_sampleCount) + "\n";
    s += QString::number(m_fileIndex) + "\n";
    s += m_searchChar + "\n";
    int ctr = 0;

    while(!t.atEnd())
    {
        QString line = t.readLine();

        if(++ctr > 5)
            s.append(line + "\n");
    }

    m_saveFile->resize(0);
    t << s;
    m_saveFile->flush();
    m_saveFile->close();
}

void AnnExtractorDialog::writeEntry()
{
    m_saveFile->open(QIODevice::Append);
    m_saveFile->write(m_imgDisplayed.toUtf8());
    m_saveFile->write(" ");
    m_saveFile->write(QByteArray::number(m_rect.width()));
    m_saveFile->write(" ");
    m_saveFile->write(QByteArray::number(m_rect.height()));
    m_saveFile->write(" ");
    m_saveFile->write(QByteArray::number(m_rect.x()));
    m_saveFile->write(" ");
    m_saveFile->write(QByteArray::number(m_rect.y()));
    m_saveFile->write(" \n");
    m_saveFile->flush();
    m_saveFile->close();
}

void AnnExtractorDialog::updateValues()
{
    m_avgWidth = (m_avgWidth * m_sampleCount + m_rect.width()) /
                 (m_sampleCount + 1);
    m_avgHight = (m_avgHight * m_sampleCount + m_rect.height()) /
                 (m_sampleCount + 1);
    m_sampleCount++;
    ui->counter_label->setText(QString::number(m_sampleCount));
}

bool AnnExtractorDialog::saveImage()
{
    QString file = m_saveDir + "/" + ui->search_char->text() + "_" +
                   QString::number(m_sampleCount) + ".jpg";

    if(!this->copyImage.save(file))
    {
        QMessageBox::information(this, tr("SAVE FAILED"),
                                 tr("Unable to save."));
        return false;
    }

    return true;
}

void AnnExtractorDialog::on_save_button_clicked()
{
    m_rubberBand->hide();

    if(!saveImage())
        return;

    writeEntry();
    updateValues();
    std::cout << "save success!" << std::endl;
}

void AnnExtractorDialog::on_previous_button_clicked()
{
    m_fileIndex--;

    if (m_fileIndex < 0)
    {
        m_fileIndex++;
        return;
    }

    display();
}

void AnnExtractorDialog::on_next_button_clicked()
{
    m_fileIndex++;

    if (m_fileIndex >= (int)this->m_fNames.size())
    {
        m_fileIndex--;
        return;
    }

    display();
}

void AnnExtractorDialog::display()
{
    cv::Mat input = cv::imread(m_fNames[m_fileIndex].toStdString(),
                               CV_LOAD_IMAGE_GRAYSCALE);
    QImage image = Util::toQt(input, QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage(image);
    QImage scaledImage = pixmap.toImage().scaled(pixmap.size() * devicePixelRatio(),
                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(devicePixelRatio());
    QPixmap *newScaledPixmap = new QPixmap(QPixmap::fromImage(scaledImage));
    ui->imageLabel->setScaledContents(true);
    //ui->imageLabel->resize(ui->imageLabel->pixmap()->size());
    ui->imageLabel->setPixmap(*newScaledPixmap);
    m_imgDisplayed =  m_fNames[m_fileIndex];
    int posPrevSlash = m_imgDisplayed.lastIndexOf("/", -1);
    //    int posPrevSlash = m_imgDisplayed.lastIndexOf("/",-(m_imgDisplayed.length() - posLastSlash +1));
    int posLastSlash = m_imgDisplayed.length();
    m_imgDisplayed = m_imgDisplayed.mid(posPrevSlash + 1,
                                        posLastSlash - posPrevSlash - 1);
    ui->out_info_label->setText(m_imgDisplayed);
    ui->imageLabel->repaint();
}
