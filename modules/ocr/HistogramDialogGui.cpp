#include "precompiled.h"

#include "ocr/HistogramDialogGui.h"
#include "ui_HistogramDialogGui.h"

HistogramDialogGui::HistogramDialogGui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistogramDialogGui)
{
    ui->setupUi(this);
}

HistogramDialogGui::~HistogramDialogGui()
{
    delete ui;
}

void HistogramDialogGui::plot(const cv::Mat &hist, const QString title)
{
    // Compute Histogram
    QCustomPlot *customPlot = ui->plot_widget;
    setWindowTitle(title);
    // binarize
    //    int range = hist.rows;
    cv::Mat tmp;
    hist.convertTo(tmp, CV_32FC1);
    // binarize
    int range;

    if(hist.rows < 2)
        range = hist.cols;

    else
        range = hist.rows;

    QVector<double> x(range), y(range);

    for (int i = 0; i < range; ++i)
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
