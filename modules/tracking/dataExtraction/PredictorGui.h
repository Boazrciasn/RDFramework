#ifndef PREDICTORGUI_H
#define PREDICTORGUI_H

#include "util.h"
#include <QWidget>
#include <opencv2/ml.hpp>

namespace Ui
{
class PredictorGui;
}

class PredictorGui : public QWidget
{
    Q_OBJECT

  public:
    explicit PredictorGui(QWidget *parent = 0);
    inline cv::Ptr<cv::ml::SVM> getSvm() {return m_svm;}
    QImage getConfMap(const QPixmap src, int roi_width, int roi_height, int step);
    QString svmFile() { return Util::fileNameWithoutPath(m_svmFile); }
    void writeSettings();
    ~PredictorGui();

  private slots:
    void load();
    void train();
    void save();
    void extractHOGFeatures();

  private:
    Ui::PredictorGui *ui;
    cv::Ptr<cv::ml::SVM> m_svm;

    QString m_posDes;
    QString m_negDes;
    QString m_svmFile;

    void readSettings();
};

#endif // PREDICTORGUI_H
