#ifndef RDFDIALOG_H
#define RDFDIALOG_H

#include <QDialog>

namespace Ui {
class RDFDialog;
}

class RDFDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RDFDialog(QWidget *parent = 0);
    ~RDFDialog();

private:
    Ui::RDFDialog *ui;
};

#endif // RDFDIALOG_H
