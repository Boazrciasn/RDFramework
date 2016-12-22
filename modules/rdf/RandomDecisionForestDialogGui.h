#ifndef RDFDIALOGGUI_H
#define RDFDIALOGGUI_H

#include <QDialog>

#include "RandomDecisionForest.h"
#include "util/ReaderGUI.h"

namespace Ui
{
class RandomDecisionForestDialogGui;
}

class RandomDecisionForestDialogGui : public QDialog
{
    Q_OBJECT
private:
    Ui::RandomDecisionForestDialogGui *ui;
    RandomDecisionForest m_forest;
    RDFParams PARAMS;
    ReaderGUI *m_trainDataReaderGUI;
    ReaderGUI *m_testDataReaderGUI;
    qint16 m_treeid;
    quint32 m_nTreesForDetection;
    bool isTestDataProcessed{};

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    void initParamValues();

    void applySobel(std::vector<cv::Mat> &images);
    void applyCanny(std::vector<cv::Mat> &images);


public:
    explicit RandomDecisionForestDialogGui(QWidget *parent = 0);
    ~RandomDecisionForestDialogGui();

public slots:
    void onTrainingBrowse();
    void onTestBrowse();
    void onTrain();
    void onTest();
    void new_tree_constructed();
    void image_at_classified_as(int index, char label);
    void resultPercetange(double accuracy);
    void printMsgToTrainScreen(QString msg);
    void printMsgToTestScreen(QString msg);

    void onLoad();
    void onSave();
    void writeSettings();
    void readSettings();

    void onNTreesChanged(int value);
    void onNTestTreesChanged(int value);
    void onMaxDepthChanged(int value);
    void onProbDistXChanged(int value);
    void onProbDistYChanged(int value);
    void onPixelsPerImageChanged(int value);
    void onMinLeafPixelsChanged(int value);
    void onMaxIterationChanged(int value);
    void onLabelCountChanged(int value);
};

#endif // RDFDIALOGGUI_H
