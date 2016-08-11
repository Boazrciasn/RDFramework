#ifndef RESIZEALLIMAGESDIALOG_H
#define RESIZEALLIMAGESDIALOG_H

#include <QDialog>

#include "Util.h"

namespace Ui
{
class ResizeAllImagesDialogGui;
}

class ResizeAllImagesDialogGui : public QDialog
{
    Q_OBJECT

  public:
    explicit ResizeAllImagesDialogGui(QWidget *parent = 0);
    ~ResizeAllImagesDialogGui();

  private slots:
    void on_pushButton_browse_clicked();
    void selectSaveDir();
    void on_pushButton_resizeall_clicked();
    void create();

  private:
    Ui::ResizeAllImagesDialogGui *ui;
    QString m_browseDir;
    QString m_saveDir;
};

#endif // RESIZEALLIMAGESDIALOG_H
