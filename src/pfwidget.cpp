#include "pfwidget.h"
#include "ui_pfwidget.h"

PFWidget::PFWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PFWidget)
{
//    m_reader = new VideoReader(this);
//    QObject::connect(m_reader, SIGNAL(processedImage(QImage)), SLOT(updatePlayerUI(QImage)));
    m_PF_executor = new PFExecutor(this);
    QObject::connect(m_PF_executor, SIGNAL(processedImage(QImage)), SLOT(updatePlayerUI(QImage)));
    ui->setupUi(this);
    ui->start_pushButton->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);

    nParticles = ui->particleCountSpinBox->value();
    nIters = ui->iterCountSpinBox->value();
    mParticleWidth = ui->particleWidthLSpinBox->value();

    ui->particlesToDisplaySlider->setMaximum(nParticles);
//    mPF = new SimplePF(&mFrame, nParticles, nIters);
//    mPF->setParticleWidth(mParticleWidth);
//    m_reader->setPF(mPF);
}

PFWidget::~PFWidget()
{
    delete ui;
}

void PFWidget::updatePlayerUI(QImage img)
{
//    mFrame = Util::toCv(frameImg,CV_8UC3);
//    cv::cvtColor(mFrame, mFrameGray, CV_BGR2GRAY);
//    mPF->setIMG(&mFrameGray);
//    mPF->run();
//    mFrameOut = mPF->getIMG();
//    QImage img = Util::toQt(mFrameOut,QImage::Format_RGB888);

    if (!img.isNull())
    {
        ui->display_label->setAlignment(Qt::AlignCenter);
        ui->display_label->setPixmap(
                    QPixmap::fromImage(img).scaled(ui->display_label->size(), Qt::KeepAspectRatio, Qt::FastTransformation)
                    );
        ui->horizontalSlider->setValue(m_PF_executor->getCurrentFrame());
    }

    int ratio = mPF->getRatioOfTop(ui->particlesToDisplaySlider->value());
    ui->top_n_ratio_label->setText(QString::number(ratio));
    ui->top_n_ratio_label->repaint();
}

QString PFWidget::getFormattedTime(int timeInSeconds)
{
    int seconds = (int) (timeInSeconds) % 60 ;
    int minutes = (int) ((timeInSeconds / 60) % 60);
    int hours   = (int) ((timeInSeconds / (60 * 60)) % 24);
    QTime t(hours, minutes, seconds);
    if (hours == 0 )
        return t.toString("mm:ss");
    else
        return t.toString("h:mm:ss");
}

void PFWidget::onHorizontalSliderPressed()
{
//    m_reader->stopVideo();
    m_PF_executor->stopVideo();
}

void PFWidget::onHorizontalSliderReleased()
{
//    m_reader->playVideo();
    m_PF_executor->playVideo();
}

void PFWidget::onHorizontalSliderMoved(int position)
{
//    m_reader->setCurrentFrame(position);
    m_PF_executor->setCurrentFrame(position);
}

void PFWidget::onSetParticleCountSliderMoved(int position)
{
    ui->particlesToDisplayLabel->setText(QString::number(position));
    mPF->setParticlesToDisplay(position);
//    mPF->showTopNParticles(position);

    int ratio = mPF->getRatioOfTop(position);
    ui->top_n_ratio_label->setText(QString::number(ratio));
    ui->top_n_ratio_label->repaint();
}

void PFWidget::onActionOpen()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a File", "", "Video File (*.*)");
    QFileInfo name = filename;
//    if (!m_reader->loadVideo(filename.toStdString()))
    if (!m_PF_executor->loadVideo(filename.toStdString()))
    {
        QMessageBox msgBox;
        msgBox.setText(" Selected video could not be opened! ");
        msgBox.exec();
    }
    else
    {
        setWindowTitle(name.fileName());
        ui->start_pushButton->setEnabled(true);
        ui->horizontalSlider->setEnabled(true);
        ui->horizontalSlider->setMaximum(m_PF_executor->getNumberOfFrames());
        qDebug() << m_PF_executor->getNumberOfFrames();
        qDebug() << m_PF_executor->getFrameHeight();
        qDebug() << m_PF_executor->getFrameWidth();

        mPF = new SimplePF(m_PF_executor->getFrameWidth(), m_PF_executor->getFrameHeight(),
                           nParticles, nIters, mParticleWidth);
        m_PF_executor->setPF(mPF);
    }
}

void PFWidget::onParticleCountChange(int value)
{
    nParticles = value;
    mPF->setNumParticles(nParticles);
    ui->particlesToDisplaySlider->setMaximum(nParticles);
}

void PFWidget::onIterationCountChange(int value)
{
    nIters = value;
    mPF->setNumIters(nIters);
}

void PFWidget::onParticleWidthChange(int value)
{
    mParticleWidth = value;
    mPF->setParticleWidth(mParticleWidth);
}


void PFWidget::onActionPlay()
{
//    m_reader->playVideo();
    m_PF_executor->playVideo();
    setPFSettingsEnabled(false);
}

void PFWidget::onActionPause()
{
//    m_reader->stopVideo();
    m_PF_executor->stopVideo();
    setPFSettingsEnabled(true);
}

void PFWidget::onActionStop()
{
//    m_reader->stopVideo();
    m_PF_executor->stopVideo();
    setPFSettingsEnabled(true);
}

void PFWidget::setPFSettingsEnabled(bool state)
{
//    ui->groupBox->setEnabled(state);
    ui->particleCountSpinBox->setEnabled(state);
    ui->iterCountSpinBox->setEnabled(state);
    ui->particleWidthLSpinBox->setEnabled(state);
}
