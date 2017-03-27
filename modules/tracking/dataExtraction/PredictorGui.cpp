#include "precompiled.h"
#include "PredictorGui.h"
#include "ui_PredictorGui.h"
#include "HOGExtactor.h"
#include "Util.h"

PredictorGui::PredictorGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PredictorGui)
{
    ui->setupUi(this);
    readSettings();
}

QImage PredictorGui::getConfMapSVM(const QPixmap src, int roi_width, int roi_height, int step)
{
    if (!m_svm)
        return src.toImage();
    cv::Mat srcImg = Util::toCv(src.toImage(), CV_8UC4);
    cv::Mat map = cv::Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC3);
    cv::Mat srcGray;
    cv::cvtColor(srcImg, srcGray, CV_RGB2GRAY);
    cv::copyMakeBorder(srcGray, srcGray, roi_height / 2, roi_height / 2, roi_width / 2, roi_width / 2, cv::BORDER_REFLECT);
    cv::HOGDescriptor *hog = new cv::HOGDescriptor();
    map.setTo(cv::Scalar(0, 255, 0));
    for (int i = step / 2; i < map.rows - 1; i += step)
    {
        for (int j = step / 2; j < map.cols - 1; j += step)
        {
            cv::Mat roi(srcGray, cv::Rect(j, i, roi_width, roi_height));
            cv::resize(roi, roi, cv::Size(64, 128));
            std::vector<float> descriptor;
            hog->compute(roi, descriptor, cv::Size(64, 128), cv::Size(16, 16));
            cv::Mat_<float> desc(descriptor);
            float decision = m_svm->predict(desc.t(), cv::noArray(), cv::ml::StatModel::RAW_OUTPUT);
            float confidence = 1.0 / (1.0 + exp(decision));
            if (decision == 1)
                confidence = 0;
            for (int row = i - step / 2; row < map.rows - 1 && row <= i + step / 2; ++row)
            {
                for (int col = j - step / 2; col < map.cols - 1 && col <= j + step / 2; ++col)
                {
                    map.at<cv::Vec3b>(row, col)[0] = 0;
                    map.at<cv::Vec3b>(row, col)[1] = 255 - confidence * 255;
                    map.at<cv::Vec3b>(row, col)[2] = confidence * 255;
                }
            }
            roi.release();
            desc.release();
            descriptor.clear();
        }
    }
    //    cv::imshow("imsh", map);
    //    cv::waitKey();
    QImage mapMask = Util::Mat2QImage(map);
    return mapMask;
}

QImage PredictorGui::getConfMapRDF(const QPixmap src, int roi_width, int roi_height, int step)
{
    if (!m_forest)
        return src.toImage();

    m_forest->setNTreesForDetection(3);
    cv::Mat srcImg = Util::toCv(src.toImage(), CV_8UC4);
    cv::Mat map = cv::Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC3);
    cv::Mat srcGray;
    cv::cvtColor(srcImg, srcGray, CV_RGB2GRAY);

    std::vector<Process*> processes;
    Sobel* proc = new Sobel(3, 3, CV_SCHARR);
    processes.push_back(proc);
    PreProcess::doBatchPreProcessSingle(srcGray,processes);

    cv::copyMakeBorder(srcGray, srcGray, roi_height / 2, roi_height / 2, roi_width / 2, roi_width / 2, cv::BORDER_REFLECT);

    map.setTo(cv::Scalar(0, 255, 0));
    for (int i = step / 2; i < map.rows - 1; i += step)
    {
        for (int j = step / 2; j < map.cols - 1; j += step)
        {
            cv::Mat roi(srcGray, cv::Rect(j, i, roi_width, roi_height));
            // TODO: pas resize params
            cv::resize(roi, roi, cv::Size(50, 50));

            int decision;
            float confidence;
            m_forest->detect(roi,decision,confidence);

            if (decision == 1)
                confidence = 0;
            for (int row = i - step / 2; row < map.rows - 1 && row <= i + step / 2; ++row)
            {
                for (int col = j - step / 2; col < map.cols - 1 && col <= j + step / 2; ++col)
                {
                    map.at<cv::Vec3b>(row, col)[0] = 0;
                    map.at<cv::Vec3b>(row, col)[1] = 255 - confidence * 255;
                    map.at<cv::Vec3b>(row, col)[2] = confidence * 255;
                }
            }
            roi.release();
        }
    }



//    cv::Mat_<float> hist = m_forest->getLayeredHist(srcGray);
//    cv::Mat_<float> conf;
//    cv::Mat lbl;
//    m_forest->getLabelAndConfMat(hist,lbl,conf);

//    cv::imshow("test",lbl);

    QImage mapMask = Util::Mat2QImage(map);
    return mapMask;
}

PredictorGui::~PredictorGui()
{
    delete ui;
}

void PredictorGui::load()
{
    m_svmFile = QFileDialog::getOpenFileName(this, QObject::tr("Open Trained SVM"),
                                             m_svmFile, QObject::tr("Trained SVM (*.xml)"));
    if (m_svmFile.contains(".xml"))
        m_svm = cv::ml::SVM::load<cv::ml::SVM>(m_svmFile.toStdString());
    emit svmChanged();
}

void PredictorGui::train()
{
    m_posDes = QFileDialog::getOpenFileName(nullptr,
                                            QObject::tr("Open pos data"), m_posDes, QObject::tr("(*.yml)"));
    if (!m_posDes.contains(".yml"))
        return;
    cv::Mat posData, negData, allData, labels;
    cv::FileStorage file(m_posDes.toStdString(), cv::FileStorage::READ);
    file["data"] >> posData;
    file.release();
    m_negDes = QFileDialog::getOpenFileName(nullptr,
                                            QObject::tr("Open neg data"), m_negDes, QObject::tr("(*.yml)"));
    if (!m_negDes.contains(".yml"))
        return;
    file.open(m_negDes.toStdString(), cv::FileStorage::READ);
    file["data"] >> negData;
    file.release();
    allData.push_back(posData);
    allData.push_back(negData);
    labels.push_back(cv::Mat::ones(posData.rows, 1, CV_32SC1));
    labels.push_back(cv::Mat::zeros(negData.rows, 1, CV_32SC1));
    m_svm = cv::ml::SVM::create();
    m_svm->setKernel(cv::ml::SVM::LINEAR);
    m_svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 10000, 1e-6));
    m_svm->train(allData , cv::ml::ROW_SAMPLE , labels);
}

void PredictorGui::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, QObject::tr("Save File"),
                                                    "/home/svm_trained.xml", QObject::tr("Trained SVM (*.xml)"));
    if (fileName.contains(".xml"))
        m_svm->save(fileName.toStdString());
}

void PredictorGui::extractHOGFeatures()
{
    HOGExtactor *hogExtr = new HOGExtactor();
    cv::Mat result = hogExtr->getResult();
    if (result.rows > 0)
    {
        QString fileName = QFileDialog::getSaveFileName(this, QObject::tr("Save File"),
                                                        "/home/posDes.yml", QObject::tr("Features Mat (*.yml)"));
        cv::FileStorage file(fileName.toStdString(), cv::FileStorage::WRITE);
        file << "data" << result;
        file.release();
    }
    delete hogExtr;
}

void PredictorGui::loadRDF()
{
    m_forest = new RandomDecisionForest();
    QString selfilter = tr("BINARY (*.bin *.txt)");
    QString fname = QFileDialog::getOpenFileName(
                        this,
                        tr("Load Forest Directory"),
                        QDir::currentPath(),
                        tr("BINARY (*.bin);;TEXT (*.txt);;All files (*.*)"),
                        &selfilter
                    );
    m_forest->loadForest(fname);
}




void PredictorGui::writeSettings()
{
    QSettings settings("VVGLab", "PredictorGui");
    settings.beginGroup("svm_path");
    settings.setValue("m_posDes", m_posDes);
    settings.setValue("m_negDes", m_negDes);
    settings.setValue("m_svmFile", m_svmFile);
    settings.endGroup();
}

void PredictorGui::readSettings()
{
    QSettings settings("VVGLab", "PredictorGui");
    settings.beginGroup("svm_path");
    m_posDes = settings.value("m_posDes", "").toString();
    m_negDes = settings.value("m_negDes", "").toString();
    m_svmFile = settings.value("m_svmFile", "").toString();
    settings.endGroup();
}
