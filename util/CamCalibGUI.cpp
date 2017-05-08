#include "CamCalibGUI.h"
#include "ui_CamCalibGUI.h"

CamCalibGUI::CamCalibGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CamCalibGUI)
{
    ui->setupUi(this);
    m_videoplayer = new VideoPlayerGui();
    m_displayImagesGUI = new DisplayGUI();
    m_splitterVert = new QSplitter(this);
    m_splitterHori = new QSplitter(this);
    m_splitterHori->addWidget(ui->leftwidget);
    m_splitterHori->addWidget(m_displayImagesGUI);
    m_splitterVert->addWidget(m_videoplayer);
    m_splitterVert->addWidget(ui->tuneableParamBox);
    m_splitterVert->setOrientation(Qt::Vertical);
    ui->left_verticalLayout->addWidget(m_splitterVert);
    ui->mainGridLayout->addWidget(m_splitterHori, 1, 0);
    QObject::connect(m_videoplayer, SIGNAL(currentFrame(QImage)), SLOT(applyIPM(QImage)));
    alpha = 0;
    beta = 0;
    gamma = 0;
    d = 0;
    f = 0;

}

CamCalibGUI::~CamCalibGUI()
{
    delete ui;
}

void CamCalibGUI::applyIPM(QImage img)
{
    img = img.convertToFormat(QImage::Format_Grayscale8);
    m_img = Util::toCv(img, CV_8UC1);
    m_displayImagesGUI->setImage(m_img);
}
