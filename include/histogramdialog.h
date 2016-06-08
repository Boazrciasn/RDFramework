#ifndef HISTOGRAMDIALOG_H
#define HISTOGRAMDIALOG_H

#include <QDialog>
#include "qcustomplot/qcustomplot.h" // the header file of QCustomPlot. Don't forget to add it to your project, if you use an IDE, so it gets compiled.
#include "opencv2/imgproc/imgproc.hpp"

namespace Ui
{
class HistogramDialog;
}

class HistogramDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit HistogramDialog(QWidget *parent = 0);
    void plot(const cv::Mat &hist, const QString title);
    ~HistogramDialog();
  private:
    Ui::HistogramDialog *ui;
};

#endif // HISTOGRAMDIALOG_H
