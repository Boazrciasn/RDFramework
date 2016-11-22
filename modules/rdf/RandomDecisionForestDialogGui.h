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

  private slots:
    void onLoad();
    void onSave();

  private:
    Ui::RandomDecisionForestDialogGui *ui;
    RandomDecisionForest m_forest;
    RDFParams PARAMS;
    ReaderGUI *m_readerGUI;
    qint16 m_treeid;

};

#endif // RDFDIALOGGUI_H
