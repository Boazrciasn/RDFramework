#include "include/TextRegionDetector.h"
#include "include/Util.h"
#include "iostream"

QVector<QRect> TextRegionDetector::detectRegions(const cv::Mat &img_bw, QWidget *parent)
{

    int leftMargin = 0, rightMargin = img_bw.cols;
//    qDebug() << "Default: " << leftMargin << "  " << rightMargin;
    getRange(img_bw,leftMargin, rightMargin,parent);
    qDebug() << "Computed: " << leftMargin << "  " << rightMargin;

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

//    Util::plot(hist,parent);

    // binarize
    QVector<int> y(range);

    for (int i=0; i < range; ++i)
    {
        if (hist.at<float>(i) < 0.10)
            y[i]  = 0;
        else
            y[i] = 1;
    }

    QVector<int> line_y = extractCoordinateFrom(y);
    y.clear();

    // for each line
    range = rightMargin;
    QVector<int> x(range);
    QVector<int> line_x;

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
            if (hist.at<float>(i) < 0.08 || i < leftMargin)
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


QVector<int> TextRegionDetector::extractCoordinateFrom(QVector<int> coord)
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

    QVector<int> result;


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

    if(start)
        result.push_back(range);

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

void TextRegionDetector::getRange(const cv::Mat &img_bw, int &leftMargin, int &rightMargin, QWidget *parent)
{
    cv::Mat region;
    float meanVal;

    for (int i=0; i < img_bw.cols; ++i)
    {
        int tmp = cv::sum(img_bw.col(i))[0];
        region.push_back(tmp);
    }

    region.convertTo(region,CV_32FC1);
    double minVals, maxVals;
    cv::minMaxLoc(region,&minVals,&maxVals);
    region = region/maxVals;

    // average filter
    for (int i=50; i < region.rows-50; ++i)
        region.at<float>(i) = cv::sum(region(cv::Range(i-50,i+50),cv::Range::all()))[0]/100;

    // calculate mean
    meanVal = cv::mean(region(cv::Range(1000,1800),cv::Range::all()))[0];
//    qDebug() << meanVal;

    // binarize
    for (int i=0; i < region.rows; ++i)
    {
        if (region.at<float>(i) < meanVal*0.6 || region.at<float>(i) > meanVal*2)
            region.at<float>(i)  = 0;
        else
            region.at<float>(i) = 1;
    }

    extractROI(region, leftMargin, rightMargin);
//    Util::plot(region,parent);
}

void TextRegionDetector::extractROI(cv::Mat &regHist, int &leftMargin, int &rightMargin)
{
    QVector<int> result;

    bool start = false;
    for (int i=0; i < regHist.rows; ++i)
    {
        if (!start && regHist.at<float>(i) == 1)
        {
            result.push_back(i);
            start = true;
        }else if(start && regHist.at<float>(i) == 0){
            result.push_back(i);
            start = false;
        }
    }

    if(start)
        result.push_back(regHist.rows);

    leftMargin = result[0];
    rightMargin = result[1];

    int range = rightMargin - leftMargin;

    for (int i = 2; i < result.size(); i +=2)
        if(range < (result[i+1] - result[i]))
        {
            rightMargin = result[i+1];
            leftMargin = result[i];
            range = rightMargin - leftMargin;
        }
}
