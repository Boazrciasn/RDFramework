#ifndef RDFDIALOG_H
#define RDFDIALOG_H

#include <QDialog>
#include "RandomDecisionForest.h"

namespace Ui {
class RDFDialog;
}

class RDFDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RDFDialog(QWidget *parent = 0);
    ~RDFDialog();

public slots:
    void on_loadTrainData_button_clicked();
    void on_loadTestData_button_clicked();
    void on_train_button_clicked();
    void on_test_button_clicked();
    void new_tree_constructed();
    void image_at_classified_as(int index,char label);
    void resultPercetange(double accuracy);

private:
    RDFParams PARAMS;
    Ui::RDFDialog *ui;
    RandomDecisionForest *m_forest;
    qint16 m_treeid;

};

#endif // RDFDIALOG_H
