#ifndef RESIZEALLIMAGESDIALOG_H
#define RESIZEALLIMAGESDIALOG_H

#include <QDialog>

#include "Util.h"

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
    void selectSaveDir();
    void on_pushButton_resizeall_clicked();
    void create();

private:
    Ui::ResizeAllImagesDialog *ui;
    QString m_browseDir;
    QString m_saveDir;
};

#endif // RESIZEALLIMAGESDIALOG_H
