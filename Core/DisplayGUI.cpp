#include "Core/DisplayGUI.h"
#include "ui_DisplayGUI.h"
#include "SignalSenderInterface.h"
#include "Util.h"

DisplayGUI::DisplayGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayGUI)
{
    ui->setupUi(this);
    ui->horizontalScrollBar_imageIndex->setEnabled(false);
}

void DisplayGUI::setImageSet(std::vector<cv::Mat> &images)
{
    m_images = images;
    if (m_images.empty())
    {
        SignalSenderInterface::instance().printsend("Images are not Loaded");
    }
    else
    {
        m_imagesExists = true;
        ui->horizontalScrollBar_imageIndex->setMaximum(m_images.size()-1);
        display();
        ui->horizontalScrollBar_imageIndex->setEnabled(true);
    }
}

DisplayGUI::~DisplayGUI()
{
    delete ui;
}

void DisplayGUI::display()
{
    cv::Mat img = m_images[m_fileIndex];
    QImage image = Util::toQt(img, QImage::Format_RGB888) ;
    QPixmap pixmap = QPixmap::fromImage(image);
    QImage scaledImage = pixmap.toImage().scaled(pixmap.size() * devicePixelRatio(), Qt::IgnoreAspectRatio,
                                                 Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(devicePixelRatio());
    QPixmap *newScaledPixmap = new QPixmap(QPixmap::fromImage(scaledImage));
    ui->label_Display->setScaledContents(true);
    ui->label_Display->setPixmap(*newScaledPixmap);
    ui->label_Display->resize(ui->label_Display->pixmap()->size());
}

void DisplayGUI::onSliderMove(int value)
{
    m_fileIndex = value;
    display();
}
