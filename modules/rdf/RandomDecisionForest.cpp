#include "precompiled.h"
#include <ctime>

#include "RandomDecisionForest.h"
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

cv::Mat_<float> RandomDecisionForest::getLayeredHist(cv::Mat &roi)
{
    cv::Mat padded_roi;
    cv::copyMakeBorder(roi, padded_roi, m_params.probDistY, m_params.probDistY,
                       m_params.probDistX, m_params.probDistX, cv::BORDER_CONSTANT);

    int nRows = padded_roi.rows;
    int nCols = padded_roi.cols;
    auto nTrees = m_forest.size();
    int labelCount = m_params.labelCount;

    // for each pix we alocate LABEL_COUNT slots to keep hist
    // therefore, ROW is the same COL is COL*LABEL_COUNT
    cv::Mat_<float> layeredHist = cv::Mat_<float>::zeros(roi.rows, roi.cols*labelCount);

    for(int row = 0; row < nRows; ++row)
        for(int col = 0; col < nCols; ++col)
        {
            if (roi.at<uchar>(row, col) == 0)
                continue;

            Pixel px;
            // Since we are sending padded roi we should add probDistX & probDistY to px.position
            px.position = cv::Point(row + m_params.probDistY, col + m_params.probDistX);

            for(size_t i = 0; i < nTrees; ++i)
                layeredHist(cv::Range(row,row + 1),cv::Range(col*labelCount,col*(labelCount+1)))
                        += m_forest[i]->getProbHist(px,padded_roi);
        }

    // normalize layeredHist
    layeredHist /= nTrees;
    return layeredHist;
}


void RandomDecisionForest::getLabelAndConfMat(cv::Mat_<float> &layeredHist,
                                              cv::Mat_<uchar> &labels, cv::Mat_<float> &confs)
{
    int labelCount = m_params.labelCount;
    int nRows = layeredHist.rows;
    int nCols = layeredHist.cols/labelCount;

    for (int row = 0; row < nRows; ++row)
        for (int col = 0; col < nCols; ++col) {
            cv::Mat_<float> tmpProbHist = layeredHist(cv::Range(row,row + 1),cv::Range(col*labelCount,col*(labelCount+1)));
            double max;
            cv::Point max_loc;
            cv::minMaxLoc(tmpProbHist, NULL, &max, NULL, &max_loc);

            // Set Pixel label & confidance
            labels(row,col) = max_loc.x;
            confs(row,col) = max;
        }
}


//void RandomDecisionForest::getProbDistributionPerPixelMat(cv::Mat &roi, cv::Mat_<uchar> &labels,
//                                              cv::Mat_<float> &confs, uchar &label, float &conf)
//{
//    conf = cv::Mat_<float>::zeros(roi.rows, roi.cols);
//    labels = cv::Mat_<uchar>::zeros(roi.rows, roi.cols);

//    cv::Mat padedRoi;

//    // TODO: Add zero pading
//    int nRows = roi.rows;
//    int nCols = roi.cols;

//    int rangeRows = nRows - m_params.probDistY;
//    int rangeCols = nCols - m_params.probDistX;

//    // probHist is used for ROI label & confidance computation
//    cv::Mat probHist = cv::Mat::zeros(1, labelCount, CV_32FC1);

//    for(int row = m_params.probDistY; row < rangeRows; ++row)
//        for(int col = m_params.probDistX; col < rangeCols; ++col)
//        {
//            auto intensity = test_image.at<uchar>(row, col);
//            if (intensity == 0)
//                continue;

//            Pixel px;
//            px.position = cv::Point(row,col);
//            // tmpProbHist is used for Pixel label & confidance computation
//            cv::Mat tmpProbHist = cv::Mat::zeros(1, labelCount, CV_32FC1);

//            auto nTrees = m_forest.size();
//            for(auto i = 0; i < nTrees; ++i)
//                tmpProbHist += m_forest[i]->getProbHist(px,roi);

//            // Add to probHist
//            // probHist is used for ROI label & confidance computation
//            // tmpProbHist is used for Pixel label & confidance computation
//            probHist += tmpProbHist;

//            //Normalize the Histrograms
//            float sum = cv::sum(tmpProbHist)[0];
//            if( sum != 0 )
//                tmpProbHist /= sum;

//            double max;
//            cv::Point max_loc;
//            cv::minMaxLoc(tmpProbHist, NULL, &max, NULL, &max_loc);


//            // Set Pixel label & confidance
//            labels(row,col) = max_loc.y; // TODO: might be max_loc.x check it
//            confs(rof,col) = max;
//        }

//    //Normalize the Histrograms
//    float sum = cv::sum(probHist)[0];
//    if( sum != 0 )
//        probHist /= sum;

//    double max;
//    cv::Point max_loc;
//    cv::minMaxLoc(tmpProbHist, NULL, &max, NULL, &max_loc);

//    // Set ROI label & confidance
//    label = max_loc.y; // TODO: might be max_loc.x check it
//    conf = max;
//}
