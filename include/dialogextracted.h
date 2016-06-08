#ifndef DIALOGEXTRACTED_H
#define DIALOGEXTRACTED_H

#include <QDialog>
namespace Ui
{
class DialogExtracted;
}

class DialogExtracted : public QDialog
{
    Q_OBJECT

  public:
    explicit DialogExtracted(QWidget *parent = 0);
    void setLabel(QImage &image);
    ~DialogExtracted();

  private:
    Ui::DialogExtracted *ui;
};

#endif // DIALOGEXTRACTED_H
