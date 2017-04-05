#include "Feature.h"

QVector<MatFeature> Feature::features;
QVector<QString> Feature::features_str;
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

    max_w = 4;
    max_h = 4;

    float ratio = 2.0f/(max_w*max_h);

    // point
    MatFeature pnt = MatFeature::ones(1, 1);

    // edges
    MatFeature edge = MatFeature::ones(max_h, max_w);
    edge(cv::Range(max_h / 2, max_h), cv::Range::all()) = -1;
    edge *= ratio;


    // lines
    MatFeature line = MatFeature::ones(max_h, max_w);
    line(cv::Range(max_h / 3, (max_h / 3) + max_h/2), cv::Range::all()) = -1;
    line *= ratio;


    // four-rectangle
    MatFeature rect = MatFeature::ones(max_h,max_w);
    rect(cv::Range(0, max_h / 2), cv::Range(max_w/ 2, max_w)) = -1;
    rect(cv::Range(max_h / 2, max_h), cv::Range(0, max_w / 2)) = -1;
    rect *= ratio;

    // Sobel
    MatFeature sobel_edge = (MatFeature(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);


    features.push_back(pnt);
    features_str.push_back(getVisual(pnt));

    features.push_back(edge);
    features_str.push_back(getVisual(edge));

    features.push_back(edge.t());
    features_str.push_back(getVisual(edge.t()));

    features.push_back(line);
    features_str.push_back(getVisual(line));

    features.push_back(line.t());
    features_str.push_back(getVisual(line.t()));

    features.push_back(rect);
        features_str.push_back(getVisual(rect));


//    features.push_back(sobel_edge);
//    features_str.push_back(getVisual(sobel_edge));

//    features.push_back(sobel_edge.t());
//    features_str.push_back(getVisual(sobel_edge.t()));
}

QString Feature::getVisual(const MatFeature feature)
{
    QString visual = "";
    auto h = feature.rows;
    auto w = feature.cols;

    for (int row = 0; row < h; ++row) {
        visual.append(getSeperator(w));
        for (int col = 0; col < w; ++col)
            if(feature(row,col) < 0)
                visual.append("| *  ");
            else if(feature(row,col) > 0)
                visual.append("| + ");
            else
                visual.append("| -- ");
        visual.append("|\n");
    }
    visual.append(getSeperator(w));
    return visual;
}

QString Feature::getSeperator(const int width)
{
    QString seperator = "\n";
    for (int i = 0; i < width; ++i) {
        seperator.prepend(" ---- ");
    }
    return seperator;
}
