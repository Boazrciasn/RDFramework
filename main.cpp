#include "src/precompiled.h"

#include "include/mainwindow.h"
#include "include/Reader.h"
#include "include/RandomDecisionForest.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    MainWindow w;


    /*  TEST SERIALIZER SAVE
    cv::Mat hist = cv::Mat::zeros(5, 25, cv::DataType<float>::type);
    int nRows = hist.rows;
    int nCols = hist.cols;
    for(int i=0; i<nRows; ++i)
    {
        auto *pRow = (float *)hist.ptr(i);
        for(int j=0; j<nCols; ++j, ++pRow)
            *pRow = j+i;
    }

    Node nd(3,true);
    nd.m_hist = hist;
    nd.m_tau = 5;
    nd.m_teta1 = Coor(5,8);
    nd.m_teta2 = Coor(7,9);

    std::ofstream file("file.bin", std::ios::binary);
    cereal::BinaryOutputArchive ar(file);
    ar(nd);
    */

    /*     TEST SERIALIZER LOAD

  Node mnd;
        {
            std::ifstream file("file.bin", std::ios::binary);
            cereal::BinaryInputArchive ar(file);
            ar(mnd);
        }

        qDebug() << QString::number(mnd.m_id)
                 << "-" << mnd.m_isLeaf
                 << "-" << QString::number(mnd.m_tau)
                 << "-(" << QString::number(mnd.m_teta1.m_dx)
                 << "-" << QString::number(mnd.m_teta1.m_dy) << ")-"
                 << "-(" << QString::number(mnd.m_teta2.m_dx)
                 << "-" << mnd.m_teta2.m_dy << ")" ;
       printHist(mnd.m_hist);

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


    QDesktopWidget dw;
    int xx=dw.width()*0.3;
    int yy=dw.height()*0.9;
    w.setFixedSize(xx,yy);
    w.show();



//    cv::Mat img = rdf->imagesVector.at(0);

//    cv::Mat img2 = cv::Mat::ones(img.rows,img.cols,img.type())*200;

//    std::vector<cv::Mat> images(3);
//    images.at(0) = img;
//    images.at(1) = img2;
//    images.at(2) = img2;

//    cv::Mat codedImage;
//    cv::merge(images,codedImage);

//    cv::imshow("test", codedImage);
//    qDebug()<<"Test image size :"<<codedImage.rows << "x" << img.cols;
//    qDebug()<<"Test image channels : "<< codedImage.type();

//    Util::calcWidthHeightStat("/home/vvglab/Desktop/AnnotationResults");

    w.show();
    //return 0;

//    cv::Mat mat = cv::Mat::ones(1,100,CV_32FC1);
//    for (int i = 0; i < 100; ++i) {
//        mat.at<float>(0,i) = (i%10)/10.0f;
//    }

//    Util::plot(mat,w.parentWidget(),"before");

//    QString word;
//    float conf;
//    Util::getWordWithConfidance(mat, 26, word, conf);
//    Util::plot(mat,w.parentWidget(),"after");
    return app.exec();
}
