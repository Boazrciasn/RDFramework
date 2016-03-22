#include "include/TextRegionDetector.h"
#include "include/Util.h"
#include "iostream"

QVector<QRect> TextRegionDetector::detectRegions(const cv::Mat &img_bw, QWidget *parent)
{
    // calculate histogram
    QVector<QRect> result;
    int range = img_bw.rows;
    cv::Mat hist;

    for (int i=0; i < range; ++i)
    {
        int tmp = cv::sum(img_bw.row(i))[0];
        hist.push_back(tmp);
    }

    hist.convertTo(hist,CV_32FC1);
    double minVal, maxVal;
    cv::minMaxLoc(hist,&minVal,&maxVal);
    hist = hist/maxVal;

    Util::plot(hist,parent);

    // binarize
    QVector<double> y(range);

    for (int i=0; i < range; ++i)
    {
        if (hist.at<float>(i) < 0.10)
            y[i]  = 0;
        else
            y[i] = 1;
    }

    QVector<double> line_y = extractCoordinateFrom(y);
    y.clear();

    // for each line
    range = img_bw.cols - 400;
    QVector<double> x(range);
    QVector<double> line_x;

    for (int row = 0; row < line_y.size()-1; row+=2) {
        hist.release();

        for (int i=0; i < range; ++i)
        {
            int tmp = cv::sum(img_bw(cv::Range(line_y[row],line_y[row+1]),cv::Range(i,i+1)))[0];
            hist.push_back(tmp);
        }

        hist.convertTo(hist,CV_32FC1);
        cv::minMaxLoc(hist,&minVal,&maxVal);
        hist = hist/maxVal;

        for (int i=10; i < range-10; ++i)
        {
            hist.at<float>(i) = cv::sum(hist(cv::Range(i-10,i+10),cv::Range::all()))[0]/20;
        }

//        Util::plot(hist,parent);

        for (int i=0; i < range; ++i)
        {
            if (hist.at<float>(i) < 0.08)
                x[i]  = 0;
            else
                x[i] = 1;
        }

        line_x = extractCoordinateFrom(x);
        int w, h = line_y[row+1] - line_y[row];

        for (int i = 0; i < line_x.size(); i+=2) {
            w = line_x[i+1]-line_x[i];
            result.push_back(QRect(line_x[i],line_y[row],w,h));
        }

        line_x.clear();
    }

    qDebug()<<"I am done " << result.size();
    return result;
}


QVector<double> TextRegionDetector::extractCoordinateFrom(QVector<double> coord)
{
    // look for tmp reagion to merge
    int range = coord.size();
    int tmp = 7;
    for (int i=tmp; i < range; ++i)
        if (coord[i] == 1)
            for (int j= i-tmp; j < i; ++j)
                if (coord[j] == 1)
                    for (; j < i; ++j)
                        coord[j] = 1;

    QVector<double> result;


    //add dumy
    result.push_back(0);

    bool start = false;
    for (int i=0; i < range; ++i)
    {
        if (!start && coord[i] == 1)
        {
            result.push_back(i);
            start = true;
        }else if(start && coord[i] == 0){
            result.push_back(i);
            start = false;
        }
    }

    //add dumy
    result.push_back(range);

    // artificial dilation
    int size = 15;
    for (int i = 1; i < result.size()-1; i+=2) {
//        int size = result[i+1] - result[i];

        if(result[i-1] > (result[i]-size-5))
            result[i] = result[i-1] + 5;
        else
            result[i] -= size;

        for (int j = 0; j < size; j++) {
            if((result[i+1]+j) == (result[i+2]-j)){
                result[i+1] += (j-5);
                break;
            }else if(j == size-1){
                result[i+1] += j;
            }
        }
    }

    //remove dumy
    result.pop_back();
    result.pop_front();
    return result;
}
