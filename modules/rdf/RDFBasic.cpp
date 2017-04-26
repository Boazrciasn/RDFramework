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


