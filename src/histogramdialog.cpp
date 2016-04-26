#include "precompiled.h"

#include "include/histogramdialog.h"
#include "ui_histogramdialog.h"

HistogramDialog::HistogramDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistogramDialog)
{
    ui->setupUi(this);
}

HistogramDialog::~HistogramDialog()
{
    delete ui;
}

void HistogramDialog::plot(const cv::Mat &hist)
{
    // Compute Histogram
    QCustomPlot * customPlot = ui->plot_widget;

    // binarize
    int range = hist.rows;
    QVector<double> x(range), y(range);

    for (int i=0; i < range; ++i)
    {
        x[i]  = i;
        y[i] = hist.at<float>(i);
    }

    // plot results
    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0, range);
    customPlot->yAxis->setRange(0, 1);
    customPlot->replot();
}
