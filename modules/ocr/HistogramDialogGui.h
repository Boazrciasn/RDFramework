#ifndef HISTOGRAMDIALOGGUI_H
#define HISTOGRAMDIALOGGUI_H

#include <QDialog>
#include "qcustomplot.h" // the header file of QCustomPlot. Don't forget to add it to your project, if you use an IDE, so it gets compiled.
#include "opencv2/imgproc/imgproc.hpp"

namespace Ui
{
class HistogramDialogGui;
}

class HistogramDialogGui : public QDialog
{
    Q_OBJECT

  public:
    explicit HistogramDialogGui(QWidget *parent = 0);
    void plot(const cv::Mat &hist, const QString title);
    ~HistogramDialogGui();
  private:
    Ui::HistogramDialogGui *ui;
};

#endif // HISTOGRAMDIALOGGUI_H
