#ifndef RDFDIALOG_H
#define RDFDIALOG_H

#include <QDialog>

#include "RandomDecisionForest.h"

namespace Ui
{
class RDFDialog;
}

class RDFDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit RDFDialog(QWidget *parent = 0);
    ~RDFDialog();

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
    RDFParams PARAMS;
    Ui::RDFDialog *ui;
    rdf_ptr m_forest;
    qint16 m_treeid;

};

#endif // RDFDIALOG_H
