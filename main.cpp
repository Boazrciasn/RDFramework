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

<<<<<<< Updated upstream
=======
    */

//     QString tempdir = "/Users/barisozcan/Documents/AnnotationResults";
//     Util::convertToOSRAndBlure(tempdir,tempdir,11);

   /*
    //QString tempdir = "/Users/barisozcan/Documents/Development/AnnotationResults_ksize_4";
    QString tempdir = "/home/mahiratmis/Desktop/AnnotationResults";
    //    QString tempdir = "/home/vvglab/Desktop/ImageCLEF/AnnotationResults";
    RandomDecisionForest* rdf = new RandomDecisionForest(5,5);
    int no_of_trees = 15;
    rdf->setNumberofTrees(no_of_trees);
    rdf->setTrainPath(tempdir);
    rdf->readTrainingImageFiles();
    rdf->trainForest();
    // Util::convertToOSRAndBlure(tempdir,tempdir,4);
    //    qDebug() << " CloudSize = " << rdf->pixelCloudSize();
    //    //rdf->printPixelCloud();
    //    rdf->train();
    // rdf->printTree();
    //    vector<Pixel*> res;
    //    QString filename = tempdir + "/a/a_0.jpg";
    //    ImageInfo* img_inf = new ImageInfo('a',0);
    //    rdf->imageToPixels(res,filename,img_inf);
    //    if(rdf->test(res,'a'))
    //        qDebug() << "YEAYYYYY";
    //    else
    //        qDebug() << "COME ONNN";
    //    vector<Pixel*> res2;
    //    filename = tempdir + "/b/b_10.jpg";
    //    img_inf = new ImageInfo('b',10);
    //    rdf->imageToPixels(res2,filename,img_inf);
    //    if(rdf->test(res,'b'))
    //        qDebug() << "YEAYYYYY";
    //    else
    //        qDebug() << "COME ONNN";
    rdf->printTree(rdf->m_forest[0]);
    //rdf->printPixelCloud();
    // TEST :
    auto test_px = rdf->m_pixelCloud[6001];
    qDebug()<<"test pixel info : " << test_px->imgInfo->label;
    auto leaf = rdf->getLeafNode(test_px, 0, rdf->m_forest[0].m_tree);
    cv::Mat hist_total = cv::Mat::zeros(leaf.hist.size(),leaf.hist.type());
    hist_total+= leaf.hist;
    for (int i = 1; i < no_of_trees; ++i)
    {
        auto leaf = rdf->getLeafNode(test_px, 0, rdf->m_forest[i].m_tree);
        hist_total+= leaf.hist;
    }
    rdf->printHistogram(hist_total);
    rdf->printHistogram(leaf.hist);
   */


//    auto test_px = rdf->pixelCloud[6001];
//    qDebug()<<"test pixel info : " << test_px->imgInfo->label;
//    auto leaf = rdf->getLeafNode(test_px, 0, rdf->m_forest[0].m_tree);
//    cv::Mat hist_total = cv::Mat::zeros(leaf.hist.size(),leaf.hist.type());
//    hist_total+= leaf.hist;
//    for (int i = 1; i < no_of_trees; ++i)
//    {
//        auto leaf = rdf->getLeafNode(test_px, 0, rdf->m_forest[i].m_tree);
//        hist_total+= leaf.hist;
//        rdf->printHistogram(leaf.hist);
//    }
//    rdf->printHistogram(hist_total);
>>>>>>> Stashed changes

    QApplication app(argc, argv);
    MainWindow w;

    QDesktopWidget dw;
    int xx=dw.width()*0.3;
    int yy=dw.height()*0.9;
    w.setFixedSize(xx,yy);

    w.show();
    return app.exec();
}
