#include "precompiled.h"
#include <ctime>
#include <QApplication>

#include "RandomDecisionForest.h"
#include "3rdparty/pcg-cpp-0.98/include/pcg_random.hpp"
#include "3rdparty/pcg-cpp-0.98/include/randutils.hpp"
//#include <omp.h>

// histogram normalize ?
// getLeafNode and Test  needs rework
// given the directory of the all samples
// read subsampled part of the images into pixel cloud


bool RandomDecisionForest::trainForest()
{
    if (m_DS.m_ImagesVector.size() == 0)
        return false;
    //#pragma omp parallel for num_threads(8)

    for (int i = 0; i < m_params.nTrees; ++i)
    {
        uint64_t seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        randutils::seed_seq_fe128 seeder{seed};
        pcg32 rng;
        rng.seed(seeder);
        SignalSenderInterface::instance().printsend("Tree number " + QString::number(i + 1) + " is being trained");
        auto &rdt = m_trees[i];
        rdt.setDataSet(&m_DS);
        rdt.setParams(&m_params);
        rdt.setGenerator(rng);
        rdt.setSignalInterface(&m_signalInterface);
        SignalSenderInterface::instance().printsend("Train...") ;
        rdt.train();
        if (rdt.isPixelSizeConsistent())
            SignalSenderInterface::instance().printsend("Pixel size is consistent at the leaves!");
        else
            SignalSenderInterface::instance().printsend("Pixel size is not consistent at the leaves!") ;
        SignalSenderInterface::instance().printsend("Tree " + QString::number(i + 1) + " Constructed.") ;
        qApp->processEvents();
    }
    SignalSenderInterface::instance().printsend("Forest Size : " + QString::number(m_trees.size()));
    return true;
}

void RandomDecisionForest::detect(cv::Mat &roi, int &label, float &conf)
{
    cv::Mat_<float> probHist;
    roi = 255 - roi;
    getCumulativeProbHist(probHist, getLayeredHist(roi));
    double max;
    cv::Point max_loc;
    cv::minMaxLoc(probHist, NULL, &max, NULL, &max_loc);
    label = max_loc.x;
    conf = max;
}


cv::Mat_<float> RandomDecisionForest::getLayeredHist(cv::Mat &roi)
{
    cv::Mat padded_roi;
    //FIX ME: refactor code.
    cv::copyMakeBorder(roi, padded_roi, m_params.probDistY, m_params.probDistY,
                       m_params.probDistX, m_params.probDistX, cv::BORDER_CONSTANT);
    int nRows = roi.rows;
    int nCols = roi.cols;
    int labelCount = m_params.labelCount;
    // for each pix we alocate LABEL_COUNT slots to keep hist
    // therefore, ROW is the same COL is COL*LABEL_COUNT
    cv::Mat_<float> layeredHist = cv::Mat_<float>::zeros(roi.rows, roi.cols * labelCount);
    for (int row = 0; row < nRows; ++row)
        for (int col = 0; col < nCols; ++col)
        {
            if (roi.at<uchar>(row, col) == 0)
                continue;
            Pixel px;
            // Since we are sending padded roi we should add probDistX & probDistY to px.position
            px.position = cv::Point(col + m_params.probDistX, row + m_params.probDistY);
            for (size_t i = 0; i < m_nTreesForDetection; ++i)
            {
                auto tmp = m_trees[i].getProbHist(px, padded_roi);
                for (int var = 0; var < labelCount; ++var)
                    layeredHist(row, col * labelCount + var) += tmp(var);
            }
        }
    // normalize layeredHist assuming leaf nodes are already normalized
    layeredHist /= m_nTreesForDetection;
    return layeredHist;
}

void RandomDecisionForest::getCumulativeProbHist(cv::Mat_<float> &probHist, const cv::Mat_<float> &layeredHist)
{
    int labelCount = m_params.labelCount;
    int nRows = layeredHist.rows;
    int nCols = layeredHist.cols / labelCount;
    probHist = cv::Mat_<float>::zeros(1, labelCount);
    for (int row = 0; row < nRows; ++row)
        for (int col = 0; col < nCols; ++col)
        {
            auto tmp = layeredHist(cv::Range(row, row + 1), cv::Range(col * labelCount, (col + 1) * labelCount));
            //            std::cout<< "tmp: " << tmp << std::endl;
            probHist = probHist + tmp;
            //            std::cout<< "probHist: " << probHist << std::endl;
        }
    // normalize
    float sum = cv::sum(probHist)[0];
    if (sum != 0)
        probHist /= sum;
}

void RandomDecisionForest::getLabelAndConfMat(cv::Mat_<float> &layeredHist,
                                              cv::Mat &labels, cv::Mat_<float> &confs)
{
    int labelCount = m_params.labelCount;
    int nRows = layeredHist.rows;
    int nCols = layeredHist.cols / labelCount;
    labels = cv::Mat(nRows, nCols, CV_8UC3);
    labels.setTo(cv::Scalar(255, 255, 255));
    confs = cv::Mat_<float>(nRows, nCols);
    for (int row = 0; row < nRows; ++row)
        for (int col = 0; col < nCols; ++col)
        {
            cv::Mat_<float> tmpProbHist = layeredHist(cv::Range(row, row + 1), cv::Range(col * labelCount, (col + 1) * labelCount));
            double max;
            cv::Point max_loc;
            cv::minMaxLoc(tmpProbHist, NULL, &max, NULL, &max_loc);
            // Set Pixel label & confidance
            if (cv::sum(tmpProbHist)[0] == 0)
                continue;
            labels.at<cv::Vec3b>(row, col) = colorcode.colors[max_loc.x];
            confs(row, col) = max;
        }
}
