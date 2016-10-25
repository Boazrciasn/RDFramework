#include "tracking/particlefilter/ParticleFilterWidgetGui.h"
#include "ui_ParticleFilterWidgetGui.h"

ParticleFilterWidgetGui::ParticleFilterWidgetGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParticleFilterWidgetGui)
{
    m_VideoPlayer = new VideoPlayer(this);
    QObject::connect(m_VideoPlayer, SIGNAL(playerFrame(QImage)), SLOT(updatePlayerUI(QImage)));
    ui->setupUi(this);
    ui->start_pushButton->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->particlesToDisplaySlider->setEnabled(false);
    m_particleCount = ui->particleCountSpinBox->value();
    m_numIters = ui->iterCountSpinBox->value();
    m_particleWidth = ui->particleWidthLSpinBox->value();
    m_particleHeight = ui->particleHeightLSpinBox->value();
    m_histSize = ui->histogramSizespinBox->value();
    ui->particlesToDisplaySlider->setMaximum(m_particleCount);
    ui->histogramSizespinBox->setValue(m_histSize);
    m_RubberBand = new QRubberBand(QRubberBand::Rectangle, this);


    m_predictor = new PredictorGui(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_predictor);
    ui->predictorFrame->setLayout(layout);

    readSettings();
}

void ParticleFilterWidgetGui::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void ParticleFilterWidgetGui::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_VideoLodaded)
        return;

    if(m_dragging){
        QPoint delta = event->pos() - m_PointMove;
        m_RubberBand->move(m_Point + delta);
    } else
        m_RubberBand->setGeometry(QRect(m_Point, event->pos()).normalized());
}

void ParticleFilterWidgetGui::mousePressEvent(QMouseEvent *event)
{
    if (m_VideoLodaded)
    {
        m_VideoPlayer->stopVideo();

        if(event->button() == Qt::RightButton){
            if(m_RubberBand->isEnabled() || !m_RubberBand->isHidden())
            {
                m_PointMove = event->pos();
                m_dragging = true;
            }
            return;
        } else {
            if (m_RubberBand->isEnabled())
                m_RubberBand->hide();
            if (ui->display_label->underMouse())
            {
                m_Point = event->pos();
                m_RubberBand = new QRubberBand(QRubberBand::Rectangle, this);
                m_RubberBand->show();
                m_dragging = false;
            }
        }
    }
}

void ParticleFilterWidgetGui::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_VideoLodaded)
    {
        m_Point = m_RubberBand->pos();
        QPoint a = mapToGlobal(m_Point);
        QPoint b;
        if(m_dragging)
            b = mapToGlobal(m_RubberBand->pos() + QPoint(m_RubberBand->width(),m_RubberBand->height()));
        else
            b = event->globalPos();
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
        setConfidence();
        if(!m_dragging)
            dispConfMap();
    }
}

void ParticleFilterWidgetGui::onActionSaveTarget()
{
    if(!m_originalPix)
        return;
    m_RubberBand->hide();
    Target *currTarget;
//    m_originalPix = ui->display_label->pixmap();
    QPixmap targetPixMap = m_originalPix.copy(m_TargetROI);
    QString labelno = QString::number(m_TargetCount).rightJustified(4, '0');
    bool isOkPressed{};
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
        labelName = QInputDialog::getText(&msgBox, "Set Label Name", " Label :", QLineEdit::Normal, labelName, &isOkPressed);
        if (isOkPressed)
        {
            currTarget = new Target(labelName, targetPixMap.toImage(), m_histSize);
            m_TargetsVector.push_back(currTarget);
            ui->targets_ListWidget->addItem(currTarget->getLabel());
            m_TargetCount = m_TargetsVector.size();
            break;
        }
        else
            break;
    case QMessageBox::Cancel:
    default:
        break;
    }
}

void ParticleFilterWidgetGui::onActionSetupPF()
{
    std::tuple<int, int> frameSize = m_VideoPlayer->getFrameSize();
    int width;
    int height;
    std::tie(width, height) = frameSize;
    m_TargetsVector[0]->setHistSize(m_histSize);
    m_particleWidth = m_TargetsVector[0]->getWidth();
    m_particleHeight = m_TargetsVector[0]->getHeight();
    ui->particleWidthLSpinBox->setValue(m_particleWidth);
    ui->particleHeightLSpinBox->setValue(m_particleHeight);
    m_PF = new ParticleFilter(width, height, m_particleCount, m_numIters,
                              m_particleWidth, m_particleHeight, m_histSize,m_TargetsVector[0]);
    m_PF->setSVM(m_predictor->getSvm());
    m_PF->initializeParticles();
    ui->particlesToDisplaySlider->setMaximum(m_particleCount);
    m_VideoPlayer->setPF(m_PF);
}

ParticleFilterWidgetGui::~ParticleFilterWidgetGui()
{
    delete m_VideoPlayer;
    delete ui;
}

void ParticleFilterWidgetGui::updatePlayerUI(QImage img)
{
    if (!img.isNull())
    {
        ui->display_label->setAlignment(Qt::AlignCenter);
        ui->display_label->setPixmap(
                    QPixmap::fromImage(img).scaled(ui->display_label->size()));
        //        , Qt::KeepAspectRatio, Qt::FastTransformation)
        //        );

        ui->horizontalSlider->setValue(m_VideoPlayer->getCurrentFrame());
        m_originalPix = ui->display_label->pixmap()->copy();
    }
//    int ratio = m_PF->getRatioOfTop(ui->particlesToDisplaySlider->value());
//    ui->top_n_ratio_label->setText(QString::number(ratio));
//    ui->top_n_ratio_label->repaint();
}

QString ParticleFilterWidgetGui::getFormattedTime(int timeInSeconds)
{
    int seconds = (int)(timeInSeconds) % 60 ;
    int minutes = (int)((timeInSeconds / 60) % 60);
    int hours   = (int)((timeInSeconds / (60 * 60)) % 24);
    QTime t(hours, minutes, seconds);
    if (hours == 0)
        return t.toString("mm:ss");
    else
        return t.toString("h:mm:ss");
}

void ParticleFilterWidgetGui::onHorizontalSliderPressed()
{
    m_isPlaying = !m_VideoPlayer->isStopped();
    m_VideoPlayer->stopVideo();
}

void ParticleFilterWidgetGui::onHorizontalSliderReleased()
{
    m_VideoPlayer->setCurrentFrame(m_currFrame);
    if(m_PF)
        m_PF->reInitialiaze();
    if (m_isPlaying)
        m_VideoPlayer->playVideo();
}

void ParticleFilterWidgetGui::onHorizontalSliderMoved(int position)
{
    m_currFrame = position;
}

void ParticleFilterWidgetGui::onSetParticleCountSliderMoved(int position)
{
    ui->particlesToDisplayLabel->setText(QString::number(position));
    m_PF->setParticlesToDisplay(position);
    //    mPF->showTopNParticles(position);
//    int ratio = m_PF->getRatioOfTop(position);
//    ui->top_n_ratio_label->setText(QString::number(ratio));
//    ui->top_n_ratio_label->repaint();
}

void ParticleFilterWidgetGui::onActionOpen()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a File", "", "Video File (*.*)");
    QFileInfo name = filename;
    if (!m_VideoPlayer->loadVideo(filename.toStdString()))
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
        ui->horizontalSlider->setMaximum(m_VideoPlayer->getNumberOfFrames());
        std::tuple<int, int> framesize = m_VideoPlayer->getFrameSize();
        int frameWidth;
        int frameHeight;
        std::tie(frameWidth, frameHeight) = framesize;
        m_VideoLodaded = true;
    }
}

void ParticleFilterWidgetGui::onParticleCountChange(int value)
{
    m_particleCount = value;
}

void ParticleFilterWidgetGui::onIterationCountChange(int value)
{
    m_numIters = value;
}

void ParticleFilterWidgetGui::onParticleWidthChange(int value)
{
    m_particleWidth = value;
}

void ParticleFilterWidgetGui::onParticleHeightChange(int value)
{
    m_particleHeight = value;
}

void ParticleFilterWidgetGui::onHistSizeChanged(int value)
{
    m_histSize = value;
}

void ParticleFilterWidgetGui::writeSettings()
{
    QSettings settings("VVGLab", "PFilter");

    settings.beginGroup("PFSettings");
    settings.setValue("particleCountSpinBox", ui->particleCountSpinBox->value());
    settings.setValue("iterCountSpinBox", ui->iterCountSpinBox->value());
    settings.setValue("particleWidthLSpinBox", ui->particleWidthLSpinBox->value());
    settings.setValue("particleHeightLSpinBox", ui->particleHeightLSpinBox->value());
    settings.setValue("histogramSizespinBox", ui->histogramSizespinBox->value());
    settings.endGroup();
}

void ParticleFilterWidgetGui::readSettings()
{
    QSettings settings("VVGLab", "PFilter");

    settings.beginGroup("PFSettings");
    ui->particleCountSpinBox->setValue(settings.value("particleCountSpinBox",100).toInt());

    ui->iterCountSpinBox->setValue(settings.value("iterCountSpinBox",3).toInt());
    ui->particleWidthLSpinBox->setValue(settings.value("particleWidthLSpinBox",64).toInt());
    ui->particleHeightLSpinBox->setValue(settings.value("particleHeightLSpinBox",128).toInt());
    ui->histogramSizespinBox->setValue(settings.value("histogramSizespinBox",12).toInt());
    settings.endGroup();
}


void ParticleFilterWidgetGui::onActionPlay()
{
    if (m_VideoLodaded)
    {
        m_VideoPlayer->playVideo();
        setPFSettingsEnabled(false);
    }
}

void ParticleFilterWidgetGui::onActionPause()
{
    if (m_VideoLodaded)
    {
        m_VideoPlayer->stopVideo();
        setPFSettingsEnabled(true);
    }
}


void ParticleFilterWidgetGui::setPFSettingsEnabled(bool state)
{
    //    ui->groupBox->setEnabled(state);
    ui->particleCountSpinBox->setEnabled(state);
    ui->iterCountSpinBox->setEnabled(state);
    ui->particleWidthLSpinBox->setEnabled(state);
}

void ParticleFilterWidgetGui::setConfidence()
{
    m_svm = m_predictor->getSvm();
    if(!m_RubberBand->isVisible() || !m_svm)
        return;

    QPixmap targetPixMap = m_originalPix.copy(m_TargetROI);
    QImage q_roi = targetPixMap.toImage();
    cv::Mat roi = Util::toCv(q_roi, CV_8UC4);
    cv::cvtColor(roi, roi, CV_RGB2GRAY);

    cv::resize(roi,roi,cv::Size(64,128));
    std::vector<float> descriptor;
    cv::HOGDescriptor *hog = new cv::HOGDescriptor();
    hog->compute(roi, descriptor, cv::Size(64, 128), cv::Size(16, 16));
    // TODO: using descriptor and svm calculate weight for the particle

    cv::Mat_<float> desc(descriptor);
    float decision = m_svm->predict(desc.t(),cv::noArray(), cv::ml::StatModel::RAW_OUTPUT);
    float confidence = 1.0 / (1.0 + exp(decision));

    ui->roi_confidence->setText(QString::number(confidence));
    ui->roi_class->setText(QString::number(decision/std::abs(decision)));
    ui->roi_confidence->repaint();
    ui->roi_class->repaint();
}

void ParticleFilterWidgetGui::dispConfMap()
{
    if(!m_svm)
        return;
    QImage confMap = m_predictor->getConfMap(m_originalPix, m_TargetROI);
    QImage img = m_originalPix.toImage();

    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_Overlay);
    p.setOpacity(0.9);
    p.drawImage(0,0,confMap);
    p.end();
    ui->display_label->setPixmap(QPixmap::fromImage(img));
}
