#include "include/Util.h"
#include <opencv2/imgproc/imgproc.hpp>



//cv::Mat Util::toCv(const QImage &image)
//{
//    QImage   swapped = image.rgbSwapped();
//    return cv::Mat( swapped.height(), swapped.width(), CV_8UC3,
//                    const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();

//}

//QImage Util::toQt(const cv::Mat &src)
//{
//   QImage image( src.data, src.cols, src.rows, src.step, QImage::Format_Indexed8 );
//   return image;
//}

cv::Mat Util::toCv(const QImage &image)
{
    cv::Mat mat = cv::Mat(image.height(), image.width(), CV_8UC4, (uchar*)image.bits(), image.bytesPerLine());
    cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
    int from_to[] = { 0,0,  1,1,  2,2 };
    cv::mixChannels( &mat, 1, &mat2, 1, from_to, 3 );
    return mat2;
}

QImage Util::toQt(const cv::Mat &src)
{
    cv::Mat rgb;
    cv::cvtColor(src, rgb, CV_BGR2RGB);
    return QImage((const unsigned char*)(rgb.data), rgb.cols, rgb.rows, QImage::Format_RGB888);
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

