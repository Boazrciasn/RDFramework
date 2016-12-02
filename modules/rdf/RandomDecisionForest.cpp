#include "precompiled.h"
#include <ctime>

#include "RandomDecisionForest.h"
//#include <omp.h>

// histogram normalize ?
// getLeafNode and Test  needs rework
// given the directory of the all samples
// read subsampled part of the images into pixel cloud


bool RandomDecisionForest::trainForest()
{
    if(m_DS.m_ImagesVector.size() == 0)
        return false;
    double cpu_time;

    static int i = 0;
    std::random_device rd;
    std::mt19937 generator = std::mt19937(rd());
    generator.seed(++i);

    //#pragma omp parallel for num_threads(8)
    for (int i = 0; i < m_params.nTrees; ++i)
    {
        clock_t start = clock();

        emit printTrainMsg("Tree number " + QString::number(i + 1) + " is being trained");
        auto &rdt = m_trees[i];
        rdt.setDataSet(&m_DS);
        rdt.setParams(&m_params);
        rdt.setGenerator(generator);

        emit printTrainMsg("Train...") ;
        rdt.train();

        if(rdt.isPixelSizeConsistent())
            emit printTrainMsg("Pixel size is consistent at the leafs!");
        else
            emit printTrainMsg("Pixel size is not consistent at the leafs!") ;
        // TODO: save tree
        cpu_time = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
        emit treeConstructed();
        emit printTrainMsg(" Train time of the current Tree : " + QString::number(cpu_time));
    }
    emit printTrainMsg("Forest Size : " + QString::number(m_trees.size()));
    return true;
}

void RandomDecisionForest::detect(cv::Mat &roi, int &label, float &conf)
{
    cv::Mat_<float> probHist;
    getCumulativeProbHist(probHist, getLayeredHist(roi));

    double max;
    cv::Point max_loc;
    cv::minMaxLoc(probHist, NULL, &max, NULL, &max_loc);

    // Set label & confidance
    label = max_loc.x;
    conf = max;
}


cv::Mat_<float> RandomDecisionForest::getLayeredHist(cv::Mat &roi)
{
    cv::Mat padded_roi;
    cv::copyMakeBorder(roi, padded_roi, m_params.probDistY, m_params.probDistY,
                       m_params.probDistX, m_params.probDistX, cv::BORDER_CONSTANT);

    int nRows = roi.rows;
    int nCols = roi.cols;
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

            for(size_t i = 0; i < m_nTreesForDetection; ++i)
            {
                auto tmp = m_trees[i].getProbHist(px,padded_roi);
//                std::cout<< "tmp: " << tmp << std::endl;
                for (int var = 0; var < labelCount; ++var) {
                   layeredHist(row,col*labelCount + var) += tmp(var);
                }
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
    int nCols = layeredHist.cols/labelCount;
    probHist = cv::Mat_<float>::zeros(1, labelCount);

    for (int row = 0; row < nRows; ++row)
        for (int col = 0; col < nCols; ++col)
        {
            auto tmp = layeredHist(cv::Range(row,row + 1),cv::Range(col*labelCount,(col+1)*labelCount));
//            std::cout<< "tmp: " << tmp << std::endl;
            probHist = probHist + tmp;
//            std::cout<< "probHist: " << probHist << std::endl;
        }
    // normalize
    float sum = cv::sum(probHist)[0];
    if( sum != 0 )
        probHist /= sum;
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
