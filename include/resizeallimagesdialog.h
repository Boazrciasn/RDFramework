#ifndef RESIZEALLIMAGESDIALOG_H
#define RESIZEALLIMAGESDIALOG_H

#include <QDialog>

namespace Ui {
class ResizeAllImagesDialog;
}

class ResizeAllImagesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResizeAllImagesDialog(QWidget *parent = 0);
    ~ResizeAllImagesDialog();

private slots:
    void on_pushButton_browse_clicked();

    void on_pushButton_resizeall_clicked();

private:
    Ui::ResizeAllImagesDialog *ui;
    QString dir;
};

#endif // RESIZEALLIMAGESDIALOG_H
