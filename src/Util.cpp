#include "include/Util.h"

cv::Mat Util::toCv(const QImage &image)
{
    return cv::Mat(image.height(), image.width(), CV_8UC3, (void *)image.scanLine(0), image.bytesPerLine());
}

QImage Util::toQt(const cv::Mat &src)
{
    quint16 width = src.cols;
    quint16 height = src.rows;

    QImage dest = QImage(width, height, QImage::Format_RGB888);

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
    QString puncs ="!'^+%&/()=?_-}][{½$#£><@.,:;|";
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
    //check if the word is a number
    bool isNumber;
    toReturn.toInt(&isNumber);
    if (!isNumber)
    {
        return toReturn.toLower();
    }
    return "";
}

