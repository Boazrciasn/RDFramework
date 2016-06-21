#include "precompiled.h"

#include "include/mainwindow.h"
#include "include/annextractordialog.h"
#include "include/rdfdialog.h"
#include "include/resizeallimagesdialog.h"

#include "include/TextRegionDetector.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    reader = new Reader();
    ui->setupUi(this);
    ui->label->setScaledContents(true);
    connect(ui->runButton, SIGNAL (clicked()), this, SLOT (update()));
    // Shortcut to Random Desicion Forest Window
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    QObject::connect(shortcut, SIGNAL(activated()), this,
                     SLOT(on_actionRDF_triggered()));
    shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
    QObject::connect(shortcut, SIGNAL(activated()), this,
                     SLOT(on_actionResizer_triggered()));
}

// ****************************** //
// ****************************** //

void MainWindow::display()
{
    //TODO : move this part to a word/line detector UI :
    QString fileName = dir + "/" + this->fNames[fileIndex];
    //    QImage image(fileName + ".jpg");
    QImage image(fileName + ".png");
    // Testing
    cv::Mat img_bw = Util::toCv(image, CV_8UC4);
    cv::cvtColor(img_bw, img_bw, CV_BGR2GRAY);
    //    cv::threshold(img_bw, img_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    //    img_bw.convertTo(img_bw,CV_32FC1);
    //    img_bw = 255 - img_bw;
    QVector<QRect> out = TextRegionDetector::detectWordsFromLine(img_bw, this);
    //    QVector<QRect> out = TextRegionDetector::detectRegions(img_bw,this);
    QPainter qPainter(&image);
    qPainter.setBrush(Qt::NoBrush);
    qPainter.setPen(QPen(QColor(255, 0, 0), 2, Qt::SolidLine, Qt::SquareCap,
                         Qt::BevelJoin));
    qDebug() << out.size();
    for (int i = 0; i < out.size(); i++)
    {
        qPainter.drawRect(out[i]);
    }
    image.save("out.jpg");
    QPixmap pixmap = QPixmap::fromImage(image);
    QImage scaledImage = pixmap.toImage().scaled(pixmap.size() * devicePixelRatio(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(devicePixelRatio());
    QPixmap *newScaledPixmap = new QPixmap(QPixmap::fromImage(scaledImage));
    ui->label->setPixmap(*newScaledPixmap);
    ui->label->resize(ui->label->pixmap()->size());
}

// ****************************** //
// ****************************** //

MainWindow::~MainWindow()
{
    std::cout << " im out dude " << std::endl;
    delete ui;
}

void MainWindow::on_browse_clicked()
{
    dir = QFileDialog::getExistingDirectory(this, tr("Open Image Direrctory"),
                                            "/home/vvglab/Desktop/ImageCLEF2016/pages_devel"/*QDir::currentPath()*/,
                                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    fileIndex = 0;
    reader->readFromTo(dir.toStdString(), this->fNames);
    if (!this->fNames.empty())
    {
        display();
        //        mDialog = new MyDialog(this);
        //        mDialog->show();
        //        mDialog->setFNames(this->fNames,dir);
    }
}



void MainWindow::on_previous_clicked()
{
    fileIndex--;
    if (fileIndex < 0)
        fileIndex++;
    display();
}

// ****************************** //
// ****************************** //

void MainWindow::on_next_clicked()
{
    fileIndex++;
    if (fileIndex >= (int)this->fNames.size())
        fileIndex--;
    display();
}

void MainWindow::on_extractWords_clicked()
{
    QString saveDir = QFileDialog::getExistingDirectory(this,
                                                        tr("Open Image Direrctory"), QDir::currentPath(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    pageParser = new PageParser();
    int size = (int)this->fNames.size();
    for (int i = 0; i < size ; ++i)
    {
        QString fileName = dir + "/" + this->fNames[i];
        pageParser->readFromTo(fileName, this->words, this->coords);
        pageParser->cropPolygons(fileName, saveDir, this->words, this->coords);
        this->words.clear();
        this->coords.clear();
    }
    delete pageParser;
    pageParser = NULL;
}

void MainWindow::on_actionAnnotation_Extractor_triggered()
{
    AnnExtractorDialog *mDialog = new AnnExtractorDialog(this);
    mDialog->exec();
}

void MainWindow::on_actionRDF_triggered()
{
    RDFDialog *mDialog = new RDFDialog(this);
    mDialog->exec();
}

void MainWindow::on_actionResizer_triggered()
{
    ResizeAllImagesDialog *mDialog = new ResizeAllImagesDialog(this);
    mDialog->exec();
}

void MainWindow::open_displayimageswidget()
{
    DisplayImagesWidget *displayimageswidget = new DisplayImagesWidget();
    displayimageswidget->show();
}

void MainWindow::open_RDFwidget()
{
    RDFDialog *rdfwidget = new RDFDialog();
    rdfwidget->show();
}

void MainWindow::open_AnnExtwidget()
{
    AnnExtractorDialog *annextwidget = new AnnExtractorDialog();
    annextwidget->show();
}

void MainWindow::open_Resizerwidget()
{
    ResizeAllImagesDialog *resizerwidget = new ResizeAllImagesDialog();
    resizerwidget->show();
}
