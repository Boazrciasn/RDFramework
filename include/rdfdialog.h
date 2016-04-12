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

private slots:
    void on_loadTrainData_button_clicked();

    void on_loadTestData_button_clicked();

    void on_train_button_clicked();

    void on_test_button_clicked();

private:
    Ui::RDFDialog *ui;
    QString train_dir;
    QString test_dir;
    RandomDecisionForest *forest;
};

#endif // RDFDIALOG_H
