#include "precompiled.h"
#include "PredictorGui.h"
#include "ui_PredictorGui.h"
#include "HOGExtactor.h"

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
    QString svmFile = QFileDialog::getOpenFileName(this, QObject::tr("Open Trained SVM"),
                                                        QDir::currentPath(),QObject::tr("Trained SVM (*.xml)"));

    if(svmFile.contains(".xml"))
        m_svm = cv::ml::SVM::load<cv::ml::SVM>(svmFile.toStdString());
}

void PredictorGui::train()
{
    QString posDes = QFileDialog::getOpenFileName(nullptr,
                                               QObject::tr("Open pos data"),QDir::currentPath(), QObject::tr("(*.yml)"));

    if(!posDes.contains(".yml"))
        return;
    cv::Mat posData, negData, allData, labels;
    cv::FileStorage file(posDes.toStdString(), cv::FileStorage::READ);
    file["data"] >> posData;
    file.release();

    QString negDes = QFileDialog::getOpenFileName(nullptr,
                                               QObject::tr("Open neg data"), QDir::currentPath(), QObject::tr("(*.yml)"));

    if(!negDes.contains(".yml"))
        return;

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
                                                    "/home/svm_trained.xml",QObject::tr("Trained SVM (*.xml)"));
    if(fileName.contains(".xml"))
        m_svm->save(fileName.toStdString());
}

void PredictorGui::extractHOGFeatures()
{
    HOGExtactor *hogExtr = new HOGExtactor();
    cv::Mat result = hogExtr->getResult();

    if(result.rows > 0)
    {
        QString fileName = QFileDialog::getSaveFileName(this, QObject::tr("Save File"),
                                                        "/home/posDes.yml",QObject::tr("Features Mat (*.yml)"));
        cv::FileStorage file(fileName.toStdString(), cv::FileStorage::WRITE);
        file << "data" << result;
        file.release();
    }

    delete hogExtr;
}
