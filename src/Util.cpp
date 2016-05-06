#include "precompiled.h"

#include <iostream>

#include "include/Util.h"

#include "omp.h"


void openmp_deneme()
{
//    quint32 nThreads = std::thread::hardware_concurrency();
//    int nThreads = omp_get_max_threads();
}

void deneme()
{
    cv::Mat_<float> A(3, 3);
    cv::Mat_<int> B(5, 5);
    A.setTo(0);
    B.setTo(10);

    doForAllPixels<float>(A, [](float pixval) {
        return pixval + 1;
    });

    doForAllPixels<int>(B, [](int pixval) {
        return pixval * 5;
    });
}

double Util::calculateAccuracy(const std::vector<QString> &groundtruth, const std::vector<QString> &results)
{
    int nSize = groundtruth.size();
    int count = 0;
    for (int i = 0; i < nSize; ++i ) {
        if (groundtruth[i] == results[i])
            ++count;
    }
    return 100*((double)count)/nSize;
}

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
    if (toReturn[i].isDigit() || toReturn[j].isDigit() )
    {
        toReturn ="Numbers/" +toReturn;
        return toReturn;
    }

    return toReturn.toLower();
}

void Util::plot(const cv::Mat &hist, QWidget *parent, const QString title)
{
    HistogramDialog *histDialog = new HistogramDialog(parent);
    histDialog->show();
    histDialog->plot(hist,title);
}

QString Util::fileNameWithoutPath(QString& filePath)
{
    int posLastDot = filePath.lastIndexOf(".",-1);
    int posLastSlash= filePath.lastIndexOf("/",-1);
    QString fname = filePath.mid(posLastSlash+1,posLastDot-posLastSlash-1);
    filePath = filePath.mid(0,posLastSlash);
    return fname;
}

void Util::convertToOSRAndBlure(QString srcDir, QString outDir, int ksize)
{
    QString folder;
    QString file;
    outDir += "_ksize_" + QString::number(ksize);
    QDirIterator ittDir(srcDir, QDir::Dirs | QDir::NoDotAndDotDot |QDir::CaseSensitive);

    while (ittDir.hasNext())
    {
        ittDir.next();
        folder = ittDir.fileName();
        QDirIterator ittFile(srcDir + "/"+ folder,QStringList()<<"*.jpg"<<"*.jpeg"<<"*.png",QDir::Files);

        QDir dir_save(outDir + "/" + folder);
        if (!dir_save.exists())
        {
            dir_save.mkpath(".");
            if(!dir_save.exists())
                 qDebug() << "ERROR : " << dir_save << " can not be created!";
        }

        while(ittFile.hasNext())
        {
            ittFile.next();
            file = ittFile.fileName();
            cv::Mat img_bw = cv::imread(srcDir.toStdString() + "/" + folder.toStdString() + "/" + file.toStdString(), 0);
            cv::threshold(img_bw, img_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
            img_bw = 255 - img_bw;
            cv::blur(img_bw,img_bw,cv::Size(ksize,ksize));

            QImage saveQIM = Util::toQt(img_bw, QImage::Format_RGB888);
            saveQIM.save(outDir + "/" + folder + "/" + file);
        }
    }
}

void Util::calcWidthHeightStat(QString srcDir){
    float w_avrg, h_avrg;
    int count;

    QString folder;
    QString file;
    QDirIterator ittDir(srcDir, QDir::Dirs | QDir::NoDotAndDotDot |QDir::CaseSensitive) ;

    QFile outLog(srcDir + "/AverageWidthHeight.txt");
    outLog.open(QIODevice::WriteOnly);

    while (ittDir.hasNext())
    {
        ittDir.next();
        folder = ittDir.fileName();
        QDirIterator ittFile(srcDir + "/"+ folder,QStringList()<<"*.jpg"<<"*.jpeg",QDir::Files);

        w_avrg = h_avrg = 0;
        count = 0;

        while(ittFile.hasNext())
        {
            ittFile.next();
            file = ittFile.fileName();
            cv::Mat img_bw = cv::imread(srcDir.toStdString() + "/" + folder.toStdString() + "/" + file.toStdString(), 0);

            w_avrg += img_bw.cols;
            h_avrg += img_bw.rows;
            ++count;
            img_bw.release();
        }

        w_avrg /= count;
        h_avrg /= count;

        outLog.write(folder.toStdString().c_str());
        outLog.write(" ");
        outLog.write(QByteArray::number((int)(w_avrg + 0.5f)));
        outLog.write(" ");
        outLog.write(QByteArray::number((int)(h_avrg + 0.5f)));
        outLog.write(" \n");

//        QFile textFile(srcDir + "/" + folder + "/" + folder + ".txt");
//        textFile.open(QIODevice::WriteOnly);
//        textFile.write(QByteArray::number(w_avrg));
//        textFile.write(" ");
//        textFile.write(QByteArray::number(h_avrg));
//        textFile.close();

    }
    outLog.close();
}

void Util::averageLHofCol(cv::Mat &mat, const QVector<quint32> fgNumberCol)
{
    int nCols = mat.cols;

    for (int i = 0; i < nCols; ++i)
    {
        mat.col(i) = mat.col(i)/fgNumberCol[i];
    }
}

void Util::getWordWithConfidance(cv::Mat &layeredHist, int nLabel, QString &word, float &conf)
{
    // store average width and hight
    QVector<int> avgWidth(nLabel);
    QVector<int> avgHight(nLabel);
    int index;

    // source file for average values
    QString avg = "./AverageWidthHeight.txt";
    QFile input(avg);

    if(!input.open(QIODevice::ReadOnly))
        std::cout<<"Util::getWordWithConfidance failed to open file! \n";


    /*************************************/
    /*********** extract values **********/
    /*************************************/
    do {
        QString str = input.readLine();
        QStringList myStringList = str.split(' ');
        index = str[0].unicode()%'a';
        avgWidth[index] = myStringList[1].toInt();
        avgHight[index] = myStringList[2].toInt();

        // make odd number
        if(avgWidth[index]%2 == 0)
            avgWidth[index]++;
        if(avgHight[index]%2 == 0)
            avgHight[index]++;
    }while(input.canReadLine());
    input.close();
    //    for (int i = 0; i < nLabel; ++i)
    //        qDebug() << avgWidth[i] << " " << avgHight[i];

    /*************************************/
    /*********** extract peaks ***********/
    /*************************************/

    cv::Mat peaks = cv::Mat::zeros(layeredHist.rows,layeredHist.cols,layeredHist.type());
    bool isIncreasing = true;
    int cols = peaks.cols;

    // filter each label with corresponding filter
    int ksize;
    for (int i = 0; i < nLabel; ++i) {
        cv::Mat hist = layeredHist.row(i);
        ksize = avgWidth[i];
        cv::GaussianBlur(hist, hist, cv::Size(ksize,1),0,0,cv::BORDER_CONSTANT);
        layeredHist.row(i) = hist;

        // find peaks
        for (int j = 1; j < cols-1; ++j) {
            if(isIncreasing && (hist.at<float>(j)-hist.at<float>(j-1)) < 0)
            {
                peaks.at<float>(i,j) = hist.at<float>(j);
                isIncreasing = !isIncreasing;
            }
            else if(!isIncreasing && (hist.at<float>(j)-hist.at<float>(j-1)) > 0)
            {
                isIncreasing = !isIncreasing;
            }
        }
    }

//    std::cout << "Kernel: " << avgWidth[23] << std::endl;
//    std::cout << peaks.t() << std::endl;
//    std::cout << std::endl;
//    std::cout << layeredHist.row(23) << std::endl;

    /*************************************/
    /************ extract word ***********/
    /*************************************/
    QVector<int> label(cols);
    QVector<float> accuracy(cols);

//    int maxIdx;
//    float max;
//    for (int i = 0; i < cols; ++i) {
//        maxIdx = 0;
//        max = layeredHist.at<float>(0,i);
//        for (int j = 1; j < nLabel; ++j) {
//            if(max < layeredHist.at<float>(j,i))
//            {
//                maxIdx = j;
//                max = layeredHist.at<float>(maxIdx,i);
//            }
//        }

//        label[i] = maxIdx;
//        accuracy[i] = max;
//    }

    /*************************************/
    /************ expand peaks ***********/
    /*************************************/

    cv::Mat peaksRow;
    int winBw;

    for (int i = 0; i < nLabel; ++i) {
        peaksRow = peaks.row(i);
        winBw = avgWidth[i]/2;

        for (int j = 0; j < cols; ++j) {
            // if peak expand
            if(peaksRow.at<float>(j) > 0)
            {
                for (int k = 0; k < cols; ++k) {
                    if(std::abs(k-j) <= winBw && (peaks.at<float>(i,k) < peaks.at<float>(i,j))){
                        peaks.at<float>(i,k) = peaks.at<float>(i,j);
                    }
                }
            }
        }
    }



    int maxIdx;
    float max;
    for (int i = 0; i < cols; ++i) {
        maxIdx = 0;
        max = peaks.at<float>(0,i);
        for (int j = 1; j < nLabel; ++j) {
            if(max < peaks.at<float>(j,i))
            {
                maxIdx = j;
                max = peaks.at<float>(maxIdx,i);
            }
        }

        if(max == 0)
        {
            max = 0;
            maxIdx = -1;
        }
        label[i] = maxIdx;
        accuracy[i] = max;
    }


//    qDebug()<<label;

//    qDebug()<<accuracy;

    word = "";
    for (int i = 0; i < cols; ++i) {
        if(label[i] < 0)
            continue;

        // see is it greater that others or not
        bool isLabel = true;
        for (int j = 0; j < nLabel; ++j) {
            if(j == label[i])
                continue;

            if(layeredHist.at<float>(j,i) > accuracy[i])
                isLabel = false;
        }

        if(isLabel)
            word += QString((char)(label[i]+'a'));
    }


    /*  2nd method */

    word = "";
    conf = 0;
    int lbl = -2;
    int wind;
    int lblCount = 0;
    float acc = 0;
    for (int i = 0; i < cols; ++i) {
        if(label[i] != lbl)
        {
            if(lblCount > wind/3)
            {
                word += QString((char)(lbl+'a'));
                conf += acc/lblCount;
            }
            lblCount=0;
            acc = 0;
        }

        if(label[i] < 0)
            continue;

        lbl = label[i];
        wind = avgWidth[lbl];
        lblCount++;
        acc += layeredHist.at<float>(lbl,i);
    }

    if(lblCount > wind/3)
    {
        word += QString((char)(lbl+'a'));
        conf += acc/lblCount;
    }

    conf /= word.length();

//    qDebug() << "Word extracted: " << word;
}
