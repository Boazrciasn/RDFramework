#include "pfwidget.h"
#include "ui_pfwidget.h"

PFWidget::PFWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PFWidget)
{
    m_PF_executor = new PFExecutor(this);
    QObject::connect(m_PF_executor, SIGNAL(processedImage(QImage)), SLOT(updatePlayerUI(QImage)));
    ui->setupUi(this);
    ui->start_pushButton->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->particlesToDisplaySlider->setEnabled(false);
    nParticles = ui->particleCountSpinBox->value();
    nIters = ui->iterCountSpinBox->value();
    mParticleWidth = ui->particleWidthLSpinBox->value();
    ui->particlesToDisplaySlider->setMaximum(nParticles);
    m_RubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

void PFWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_VideoLodaded)
    {
        m_RubberBand->setGeometry(QRect(m_Point, event->pos()).normalized());
    }
}

void PFWidget::mousePressEvent(QMouseEvent *event)
{
    if(m_VideoLodaded)
    {
        m_PF_executor->stopVideo();
        if (m_RubberBand->isEnabled())
            m_RubberBand->hide();
        if(ui->display_label->underMouse())
        {
            m_Point = event->pos();
            m_RubberBand = new QRubberBand(QRubberBand::Rectangle, this);
            m_RubberBand->show();
        }
    }
}

void PFWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_VideoLodaded)
    {
        QPoint a = mapToGlobal(m_Point);
        QPoint b = event->globalPos();
        a = ui->display_label->mapFromGlobal(a);
        b = ui->display_label->mapFromGlobal(b);
        QPixmap OriginalPix(*ui->display_label->pixmap());
        double sx = ui->display_label->rect().width();
        double sy = ui->display_label->rect().height();
        sx = OriginalPix.width() / sx;
        sy = OriginalPix.height() / sy;
        a = QPoint(int(a.x() * sx), int(a.y() * sy));
        b = QPoint(int(b.x() * sx), int(b.y() * sy));
        m_TargetROI = QRect(a, b);
    }
}

void PFWidget::onActionSaveTarget()
{
    Target *currTarget;
    QPixmap OriginalPix(*ui->display_label->pixmap());
    QPixmap targetPixMap = OriginalPix.copy(m_TargetROI);
    QString labelno = QString::number(m_TargetCount).rightJustified(4, '0');
    QString labelName = "Human_" + labelno;
    QMessageBox msgBox;
    msgBox.setText("Would you like to track this target?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    msgBox.setIconPixmap(targetPixMap);
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::Save:
        labelName = QInputDialog::getText(&msgBox, "Set Label Name", " Label :", QLineEdit::Normal, labelName);
        currTarget = new Target(labelName,targetPixMap.toImage());
        m_TargetsVector.push_back(currTarget);
        ui->targets_ListWidget->addItem(currTarget->getLabel());
        m_TargetCount = m_TargetsVector.size();
        break;
    case QMessageBox::Cancel:
    default:
        break;
    }
}

PFWidget::~PFWidget()
{
    delete m_PF_executor;
    delete ui;
}

void PFWidget::updatePlayerUI(QImage img)
{
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
    m_PF_executor->stopVideo();
}

void PFWidget::onHorizontalSliderReleased()
{
    m_PF_executor->playVideo();
}

void PFWidget::onHorizontalSliderMoved(int position)
{
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
        ui->particlesToDisplaySlider->setEnabled(true);
        ui->horizontalSlider->setMaximum(m_PF_executor->getNumberOfFrames());
        mPF = new SimplePF(m_PF_executor->getFrameWidth(), m_PF_executor->getFrameHeight(),
                           nParticles, nIters, mParticleWidth);
        m_PF_executor->setPF(mPF);
        m_VideoLodaded = true;
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
    m_PF_executor->playVideo();
    setPFSettingsEnabled(false);
}

void PFWidget::onActionPause()
{
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
