#include "include/mainwindow.h"
#include "QApplication"
#include <QDesktopWidget>
#include "include/Reader.h"
#include "QDebug"
#include "vector"

#include "include/RandomDecisionForest.h"


int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    MainWindow w;

    //QString tempdir = "/Users/barisozcan/Documents/Development/AnnotationResults_ksize_4";
//    QString tempdir = "/home/mahiratmis/Desktop/AnnotationResults";
//    //    QString tempdir = "/home/vvglab/Desktop/ImageCLEF/AnnotationResults";
//    RandomDecisionForest* rdf = new RandomDecisionForest(5,5);
//    int no_of_trees = 15;
//    rdf->setNumberofTrees(no_of_trees);
//    rdf->setTrainPath(tempdir);
//    rdf->readTrainingImageFiles();
//    rdf->trainForest();
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

    //rdf->printTree(rdf->m_forest[0]);
    //rdf->printPixelCloud();


    // TEST :
//    auto test_px = rdf->m_pixelCloud[6001];
//    qDebug()<<"test pixel info : " << test_px->imgInfo->label;
//    auto leaf = rdf->getLeafNode(test_px, 0, rdf->m_forest[0].m_tree);
//    cv::Mat hist_total = cv::Mat::zeros(leaf.hist.size(),leaf.hist.type());
//    hist_total+= leaf.hist;
//    for (int i = 1; i < no_of_trees; ++i)
//    {
//        auto leaf = rdf->getLeafNode(test_px, 0, rdf->m_forest[i].m_tree);
//        hist_total+= leaf.hist;
//    }
//    rdf->printHistogram(hist_total);
//    rdf->printHistogram(leaf.hist);






    QDesktopWidget dw;
    int x=dw.width()*0.3;
    int y=dw.height()*0.9;
    w.setFixedSize(x,y);


//    cv::Mat img = imread("/Users/barisozcan/Downloads/sample-1.jpg",IMREAD_GRAYSCALE);
//    cv::copyMakeBorder( img, img, 20, 20, 20, 20,BORDER_CONSTANT );
//    cv::Mat img = rdf->imagesVector.at(0);
//    cv::imshow("test", img);
    w.show();
//    return 0;

    return app.exec();
}
