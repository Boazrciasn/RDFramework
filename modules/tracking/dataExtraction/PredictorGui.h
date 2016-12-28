#ifndef PREDICTORGUI_H
#define PREDICTORGUI_H

#include "Util.h"
#include <QWidget>
#include <opencv2/ml.hpp>
#include "rdf/RandomDecisionForest.h"

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
    QImage getConfMapSVM(const QPixmap src, int roi_width, int roi_height, int step);
    QImage getConfMapRDF(const QPixmap src, int roi_width, int roi_height, int step);
    QString svmFile() { return Util::fileNameWithoutPath(m_svmFile); }
    void writeSettings();
    ~PredictorGui();

    inline RandomDecisionForest* getForest() const
    {
        return m_forest;
    }

private slots:
    void load();
    void train();
    void save();
    void extractHOGFeatures();
    void loadRDF();

  signals :
    void svmChanged();

  private:
    Ui::PredictorGui *ui;
    cv::Ptr<cv::ml::SVM> m_svm{};
    RandomDecisionForest *m_forest{};

    QString m_posDes;
    QString m_negDes;
    QString m_svmFile;

    void readSettings();
};

#endif // PREDICTORGUI_H
