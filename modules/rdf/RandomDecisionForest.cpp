#include "precompiled.h"
#include <ctime>

#include "RandomDecisionForest.h"
#include "util/Reader.h"
#include "Util.h"
#include "ocr/TextRegionDetector.h"
//#include <omp.h>

// histogram normalize ?
// getLeafNode and Test  needs rework
// given the directory of the all samples
// read subsampled part of the images into pixel cloud


void RandomDecisionForest::trainForest()
{
    double cpu_time;
    //#pragma omp parallel for num_threads(8)
    for (int i = 0; i < m_params.nTrees; ++i)
    {
        clock_t start = clock();

        qDebug() << "Tree number " << QString::number(i + 1) << "is being trained" ;
        rdt_ptr trainedRDT(new RandomDecisionTree(&m_DS, &m_params));
        qDebug() << "Train..." ;
        trainedRDT->train();
        // TODO: save tree
        cpu_time = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;

        m_forest.push_back(trainedRDT);
        qDebug() << " Train time of the current Tree : " << cpu_time;
    }
    qDebug() << "Forest Size : " << m_forest.size();
}

//void RandomDecisionForest::test()
//{
//    int nImages = m_DS.m_ImagesVector.size();
//    qDebug() << "Number of Test images:" << QString::number(nImages);
//    for(auto i = 0; i < nImages; ++i)
//    {
//        QVector<quint32> fgPxNumberPerCol;
//        cv::Mat layeredImage = getLayeredHist(m_DS.m_ImagesVector[i], i,
//                                              fgPxNumberPerCol);
//        cv::Mat_<float> confidenceMat = createLetterConfidenceMatrix(layeredImage, fgPxNumberPerCol);
//        //        std::cout<<confidenceMat.row(0)<<std::endl;
//        QString word = "Hello";
//        float conf = 0;
//        //        Util::plot(confidenceMat.row('n'-'a'), m_parent, "n");
//        Util::getWordWithConfidence(confidenceMat, 26, word, conf);
//        qDebug() << "Word extracted & conf: " << word << "  " << 100 * conf;
//    }
//    //    m_accuracy = Util::calculateAccuracy(m_DS.m_testlabels, classify_res);
//    //    emit resultPercentage(m_accuracy);
//}

