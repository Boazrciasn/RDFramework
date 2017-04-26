#include "RDFBasic.h"

cv::Mat_<float> RDFBasic::getCumulativeProbHist(const cv::Mat_<float> &layeredHist)
{
    auto lblCount = m_params.labelCount;
    auto cols = layeredHist.cols;
    cv::Mat_<float> probHist = cv::Mat_<float>::zeros(1, lblCount);

    for(auto col = 0; col < cols; ++col)
        probHist(0, col%lblCount) += cv::sum(layeredHist(cv::Range::all(),cv::Range(col,col+1)))[0];

    // normalize
    auto sum = cv::sum(probHist)[0];
    if (sum != 0)
        probHist /= sum;
    return probHist;
}

void RDFBasic::getLabelAndConfMat(cv::Mat_<float> &layeredHist, cv::Mat &labels, cv::Mat_<float> &confs)
{
    auto labelCount = m_params.labelCount;
    auto nRows = layeredHist.rows;
    auto nCols = layeredHist.cols / labelCount;

    labels = cv::Mat(nRows, nCols, CV_8UC3);
    labels.setTo(cv::Scalar(255, 255, 255));
    confs = cv::Mat_<float>::zeros(nRows, nCols);

    double max;
    cv::Point max_loc;
    for (int row = 0; row < nRows; ++row)
        for (int col = 0; col < nCols; ++col)
        {
            auto tmpProbHist = layeredHist(cv::Range(row, row + 1), cv::Range(col * labelCount, (col + 1) * labelCount));
            cv::minMaxLoc(tmpProbHist, NULL, &max, NULL, &max_loc);
            labels.at<cv::Vec3b>(row, col) = colorcode.colors[max_loc.x];
            confs(row, col) = max;
        }
}
