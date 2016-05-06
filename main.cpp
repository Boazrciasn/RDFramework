#include "src/precompiled.h"

#include "include/mainwindow.h"
#include "include/Reader.h"
#include "include/RandomDecisionForest.h"

int main(int argc, char *argv[]) {


//    //    Util::calcWidthHeightStat("/home/vvglab/Desktop/AnnotationResults");
//    QString tempdir = "/home/neko/Desktop/word";
////    Util::convertToOSRAndBlure(tempdir,tempdir,11);

//    int ksize = 11;
//    cv::Mat img_bw = cv::imread("/home/neko/Desktop/word/testWord.png", 0);
//    cv::threshold(img_bw, img_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
//    img_bw = 255 - img_bw;
//    cv::blur(img_bw,img_bw,cv::Size(ksize,ksize));

//    QImage saveQIM = Util::toQt(img_bw, QImage::Format_RGB888);
//    saveQIM.save("/home/neko/Desktop/word/testWord.jpg");



    QApplication app(argc, argv);
    MainWindow w;

    QDesktopWidget dw;
    int xx=dw.width()*0.3;
    int yy=dw.height()*0.9;
    w.setFixedSize(xx,yy);

    w.show();
    return app.exec();
}
