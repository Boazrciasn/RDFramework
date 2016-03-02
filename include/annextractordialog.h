#ifndef ANNEXTRACTORDIALOG_H
#define ANNEXTRACTORDIALOG_H

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QRubberBand>
#include "include/dialogextracted.h"
#include <QDialog>

namespace Ui {
class AnnExtractorDialog;
}

class AnnExtractorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnnExtractorDialog(QWidget *parent = 0);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    ~AnnExtractorDialog();

private slots:
    void on_browse_button_clicked();

    void on_save_button_clicked();

    void on_previous_button_clicked();

    void on_next_button_clicked();

private:
    Ui::AnnExtractorDialog *ui;
    QImage copyImage;
    QString fileName;
    QPoint myPoint;
    QRubberBand *rubberBand;
    std::vector<QString> fNames;     // file names are stored
    int fileIndex;              // index of file being viewed
};

#endif // ANNEXTRACTORDIALOG_H
