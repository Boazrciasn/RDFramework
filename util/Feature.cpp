#include "Feature.h"

std::vector<MatFeature> Feature::features;
int Feature::max_w;
int Feature::max_h;

Feature::Feature()
{

}

void Feature::init(){
    static bool isInit{};
    if(isInit)
        return;
    isInit = true;

    // point
    MatFeature pnt = MatFeature::ones(1, 1);
    // edges
    MatFeature edge = MatFeature::ones(6, 6);
    edge(cv::Range(edge.rows / 2, edge.rows), cv::Range::all()) = -1;
    // lines
    MatFeature line = MatFeature::ones(6, 6);
    line(cv::Range(line.rows / 3, line.rows / 2), cv::Range::all()) = -1;
    // four-rectangle
    MatFeature rect = MatFeature::ones(6, 6);
    rect(cv::Range(0, rect.rows / 2), cv::Range(rect.cols / 2, rect.cols)) = -1;
    rect(cv::Range(rect.rows / 2, rect.rows), cv::Range(0, rect.cols / 2)) = -1;


    max_w = 6;
    max_h = 6;

    features.push_back(pnt);
    features.push_back(edge);
    features.push_back(edge.t());
    features.push_back(line);
    features.push_back(line.t());
    features.push_back(rect);
    features.push_back(rect.t());
}
