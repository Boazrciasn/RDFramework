#include "include/Util.h"
#include <opencv2/imgproc/imgproc.hpp>



cv::Mat Util::toCv(const QImage &image)
{
    QImage   swapped = image.rgbSwapped();
    return cv::Mat( swapped.height(), swapped.width(), CV_8UC3,
                    const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();

}

QImage Util::toQt(const cv::Mat &src)
{
   QImage image( src.data, src.cols, src.rows, src.step, QImage::Format_Indexed8 );
   return image;
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

