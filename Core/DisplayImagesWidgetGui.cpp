#include "precompiled.h"


#include "Core/DisplayImagesWidgetGui.h"
#include "ui_DisplayImagesWidgetGui.h"

DisplayImagesWidgetGui::DisplayImagesWidgetGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayImagesWidgetGui)
{
    ui->setupUi(this);
    m_reader = new Reader();
}

DisplayImagesWidgetGui::~DisplayImagesWidgetGui()
{
    delete ui;
}

void DisplayImagesWidgetGui::display()
{
    cv::Mat img = m_images[m_fileIndex];
    QImage image = Util::toQt(img, QImage::Format_RGB888) ;
    QPixmap pixmap = QPixmap::fromImage(image);
    QImage scaledImage = pixmap.toImage().scaled(pixmap.size() * devicePixelRatio(), Qt::IgnoreAspectRatio,
                                                 Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(devicePixelRatio());
    QPixmap *newScaledPixmap = new QPixmap(QPixmap::fromImage(scaledImage));
    ui->label->setScaledContents(true);
    ui->label->setPixmap(*newScaledPixmap);
    ui->label->resize(ui->label->pixmap()->size());
}

void DisplayImagesWidgetGui::extractWords()
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
    m_pageParser = nullptr;
}

void DisplayImagesWidgetGui::browseButton_clicked()
{
    m_dir = QFileDialog::getOpenFileName(this, tr("Open Image Directory"),
                                         QDir::currentPath());
    m_reader->readImages(m_dir, m_images, Type_MNIST);
    m_fileIndex = 0;
    if (!m_images.empty())
        display();
}

void DisplayImagesWidgetGui::prevButton_clicked()
{
    m_fileIndex--;
    if (m_fileIndex < 0)
        m_fileIndex++;
    display();
}

void DisplayImagesWidgetGui::nextButton_clicked()
{
    m_fileIndex++;
    if (m_fileIndex >= (int)m_images.size())
        m_fileIndex--;
    display();
}
