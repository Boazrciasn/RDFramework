#include "include/Util.h"
#include <opencv2/imgproc/imgproc.hpp>


cv::Mat Util::toCv(const QImage &image , int cv_type)
{
    return cv::Mat(image.height(), image.width(), cv_type, (void *)image.scanLine(0), image.bytesPerLine());
}

QImage Util::toQt(const cv::Mat &src, QImage::Format format)
{
    quint16 width = src.cols;
    quint16 height = src.rows;

    QImage dest = QImage(width, height, format);

    if(src.type() == CV_8UC3)
    {
        for(int i = 0; i < height; i++)
        {
            const quint8 *pSrc = src.ptr<quint8>(i);
            quint8 *pDest = dest.scanLine(i);
            for(int j = 0; j < width; j++)
            {
                *pDest++ = *pSrc++;
                *pDest++ = *pSrc++;
                *pDest++ = *pSrc++;
            }
        }
    }
    else if(src.type() == CV_8UC1)
    {
        for(int i = 0; i < height; i++)
        {
            const quint8 *pSrc = src.ptr<quint8>(i);
            quint8 *pDest = dest.scanLine(i);
            for (int j = 0; j < width; j++)
            {
                quint8 val = *pSrc++;
                *pDest++ = val;
                *pDest++ = val;
                *pDest++ = val;
            }
        }
    }
    return dest;
}

QString Util::cleanNumberAndPunctuation(QString toClean)
{
    QString puncs ="!'^+%&/()=?_-{}][{½$#£><@.,:;|\"§*/";
    QString toReturn="";
    int i=0;
    int j=toClean.length();

    //find head pos to trim
    while (i<j)
    {
        if (!puncs.contains(toClean[i]))
            break;
        i++;
    }

    //find tail pos to trim
    while (j>i)
    {
        j--;
        if (!puncs.contains(toClean[j]))
            break;
    }

    toReturn = toClean.mid(i,j-i+1);

    //FIXME: got to refactor the code here sorry for bold coding.
    i = 0;
    j=toReturn.length();


    //check if the word starts or ends with a number or a whole number
    bool isNumber;
    toReturn.toInt(&isNumber);


    /* FIXME: if there is a number, return "Numbers/number/.. */

/*  if (!isNumber)
    {
        return toReturn.toLower();
    }
*/
    if (toReturn[i].isDigit() || toReturn[j].isDigit() ) {

        toReturn ="Numbers/" +toReturn;
        return toReturn;
    }


    return toReturn.toLower();
}

void Util::plot(const cv::Mat &hist, QWidget *parent)
{
    HistogramDialog *histDialog = new HistogramDialog(parent);
    histDialog->show();
    histDialog->plot(hist);
}
