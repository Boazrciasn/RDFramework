#ifndef PREDICTORGUI_H
#define PREDICTORGUI_H

#include <QWidget>
#include <opencv2/ml.hpp>

namespace Ui {
class PredictorGui;
}

class PredictorGui : public QWidget
{
    Q_OBJECT

public:
    explicit PredictorGui(QWidget *parent = 0);
    inline cv::Ptr<cv::ml::SVM> getSvm(){return m_svm;}
    QImage getConfMap(const QPixmap src, QRect win);
    ~PredictorGui();

private slots:
    void load();
    void train();
    void save();
    void extractHOGFeatures();

private:
    Ui::PredictorGui *ui;
    cv::Ptr<cv::ml::SVM> m_svm;
};

#endif // PREDICTORGUI_H
