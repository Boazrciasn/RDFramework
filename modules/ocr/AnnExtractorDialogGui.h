#ifndef ANNEXTRACTORDIALOG_H
#define ANNEXTRACTORDIALOG_H

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QRubberBand>
#include "ocr/ExtractedDialogGui.h"
#include "ocr/Reader.h"
#include <QDialog>

#define FILE_HEADER 5

namespace Ui
{
class AnnExtractorDialogGui;
}

class AnnExtractorDialogGui : public QDialog
{
    Q_OBJECT

  public:
    explicit AnnExtractorDialogGui(QWidget *parent = 0);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    ~AnnExtractorDialogGui();

  private slots:
    void on_browse_button_clicked();
    void on_save_button_clicked();
    void on_previous_button_clicked();
    void on_next_button_clicked();

  private:
    bool isBrowseble();
    bool loadFiles();
    void createSaveDir();
    void createSaveFile();
    void display();

    Ui::AnnExtractorDialogGui *ui;
    Reader m_reader;
    QImage copyImage;
    QString m_imgDisplayed;
    QRect m_rect;
    QString m_Loaddir;
    QString m_saveDir;
    QString m_searchChar;
    QPoint m_Point;
    QRubberBand *m_rubberBand;
    std::vector<QString> m_fNames;     // file names are stored
    int m_fileIndex;              // index of file being viewed
    int m_sampleCount;
    int m_avgWidth;
    int m_avgHeight;
    QFile *m_saveFile;
    void writeEntry();
    void updateValues();
    bool saveImage();
    void writeHeader();
    void loadHeader();
};

#endif // ANNEXTRACTORDIALOG_H
