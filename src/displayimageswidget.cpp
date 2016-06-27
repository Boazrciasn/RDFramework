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
    QImage image(m_fNames[m_fileIndex]);
    cv::Mat img_bw = Util::toCv(image, CV_8UC4);
    cv::cvtColor(img_bw, img_bw, CV_BGR2GRAY);
    QPixmap pixmap = QPixmap::fromImage(image);
    QImage scaledImage = pixmap.toImage().scaled(pixmap.size() * devicePixelRatio(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(devicePixelRatio());
    QPixmap *newScaledPixmap = new QPixmap(QPixmap::fromImage(scaledImage));
    ui->label->setScaledContents(true);
    ui->label->setPixmap(*newScaledPixmap);
    ui->label->resize(ui->label->pixmap()->size());
}

void DisplayImagesWidget::extractWords()
{

    //TO DO : needs update.
    QString saveDir = QFileDialog::getExistingDirectory(this,
                                                        tr("Open Image Direrctory"), QDir::currentPath(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_pageParser = new PageParser();
    int size = (int)m_fNames.size();
    for (int i = 0; i < size ; ++i)
    {
        QString fileName = m_dir + "/" + m_fNames[i];
        m_pageParser->readFromTo(fileName, m_words, m_coords);
        m_pageParser->cropPolygons(fileName, saveDir, m_words, m_coords);
        m_words.clear();
        m_coords.clear();
    }
    delete m_pageParser;
    m_pageParser = NULL;
}

void DisplayImagesWidget::browseButton_clicked()
{
    m_dir = QFileDialog::getExistingDirectory(this, tr("Open Image Direrctory"),
                                              QDir::currentPath(),
                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_fileIndex = 0;
    m_reader->findImages(m_dir, m_fNames);
    if (!m_fNames.empty())
    {
        display();
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
