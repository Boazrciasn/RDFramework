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
    QObject::connect(m_videoplayer, SIGNAL(currentFrame(QImage)), SLOT(collectImage(QImage)));
    m_params.alpha = 90.;
    m_params.beta = 90.;
    m_params.gamma = 90.;
    m_params.d = 500.;
    m_params.f = 500.;
}

CamCalibGUI::~CamCalibGUI()
{
    delete ui;
}

void CamCalibGUI::collectImage(QImage img)
{
    img = img.convertToFormat(QImage::Format_Grayscale8);
    m_img = Util::toCv(img, CV_8UC1);
    m_transformedImg = applyIPM(m_img, m_params);
    m_displayImagesGUI->setImage(m_transformedImg);
}

void CamCalibGUI::alphaslider(int inpt)
{
    m_params.alpha = ((double)inpt- 90)* PI/180;
    ui->alpha_val->setText(QString::number(m_params.alpha));
    m_transformedImg = applyIPM(m_img, m_params);
    m_displayImagesGUI->setImage(m_transformedImg);
}

void CamCalibGUI::betaslider(int inpt)
{
    m_params.beta = ((double)inpt- 90)* PI/180;
    ui->beta_val->setText(QString::number(m_params.beta));
    m_transformedImg = applyIPM(m_img, m_params);
    m_displayImagesGUI->setImage(m_transformedImg);
}

void CamCalibGUI::gammaslider(int inpt)
{
    m_params.gamma = ((double)inpt- 90)* PI/180;
    ui->gamma_val->setText(QString::number(m_params.gamma));
    m_transformedImg = applyIPM(m_img, m_params);
    m_displayImagesGUI->setImage(m_transformedImg);
}

void CamCalibGUI::dslider(int inpt)
{
    m_params.d = inpt;
    ui->distance_val->setText(QString::number(m_params.d));
    m_transformedImg = applyIPM(m_img, m_params);
    m_displayImagesGUI->setImage(m_transformedImg);
}

void CamCalibGUI::fslider(int inpt)
{
    m_params.f = inpt;
    ui->focal_val->setText(QString::number(m_params.f));
    m_transformedImg = applyIPM(m_img, m_params);
    m_displayImagesGUI->setImage(m_transformedImg);
}
