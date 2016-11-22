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
    ReaderGUI *m_readerGUI;
    qint16 m_treeid;

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    void initParamValues();

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

public slots:
    void onLoad();
    void onSave();
    void writeSettings();
    void readSettings();

    void onNTreesChanged(int value);
    void onMaxDepthChanged(int value);
    void onProbDistXChanged(int value);
    void onProbDistYChanged(int value);
    void onPixelsPerImageChanged(int value);
    void onMinLeafPixelsChanged(int value);
    void onMaxIterationChanged(int value);
    void onLabelCountChanged(int value);
};

#endif // RDFDIALOGGUI_H
