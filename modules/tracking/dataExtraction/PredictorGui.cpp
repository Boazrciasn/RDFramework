#include "precompiled.h"
#include "PredictorGui.h"
#include "ui_PredictorGui.h"

PredictorGui::PredictorGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PredictorGui)
{
    ui->setupUi(this);
}

PredictorGui::~PredictorGui()
{
    delete ui;
}

void PredictorGui::load()
{

}

void PredictorGui::train()
{
    QString posDes = QFileDialog::getOpenFileName(nullptr,
                                               QObject::tr("Open pos data"),QDir::currentPath(), QObject::tr("(*.yml)"));

    cv::Mat posData, negData, allData, labels;
    cv::FileStorage file(posDes.toStdString(), cv::FileStorage::READ);
    file["data"] >> posData;
    file.release();

    QString negDes = QFileDialog::getOpenFileName(nullptr,
                                               QObject::tr("Open neg data"), QDir::currentPath(), QObject::tr("(*.yml)"));

    file.open(negDes.toStdString(),cv::FileStorage::READ);
    file["data"] >> negData;
    file.release();

    allData.push_back(posData);
    allData.push_back(negData);

    labels.push_back(cv::Mat::ones(posData.rows,1,CV_32SC1));
    labels.push_back(cv::Mat::zeros(negData.rows,1,CV_32SC1));

    m_svm = cv::ml::SVM::create();
    m_svm->setKernel(cv::ml::SVM::LINEAR);
    m_svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 10000, 1e-6));
    m_svm->train( allData , cv::ml::ROW_SAMPLE , labels );
}

void PredictorGui::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, QObject::tr("Save File"),
                                                    "/home/svm_trained.yml",QObject::tr("Trained SVM (*.xml)"));
    m_svm->save(fileName.toStdString());
}

void PredictorGui::extractHOGFeatures()
{

}
