#ifndef DIALOGEXTRACTED_H
#define DIALOGEXTRACTED_H

#include <QDialog>
namespace Ui
{
class DialogExtractedGui;
}

class DialogExtractedGui : public QDialog
{
    Q_OBJECT

  public:
    explicit DialogExtractedGui(QWidget *parent = 0);
    void setLabel(QImage &image);
    ~DialogExtractedGui();

  private:
    Ui::DialogExtractedGui *ui;
};

#endif // DIALOGEXTRACTED_H
