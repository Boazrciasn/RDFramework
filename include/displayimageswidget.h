#ifndef DISPLAYIMAGESWIDGET_H
#define DISPLAYIMAGESWIDGET_H

#include <QWidget>

namespace Ui
{
class DisplayImagesWidget;
}

class DisplayImagesWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit DisplayImagesWidget(QWidget *parent = 0);
    ~DisplayImagesWidget();

  private:
    Ui::DisplayImagesWidget *ui;
};

#endif // DISPLAYIMAGESWIDGET_H
