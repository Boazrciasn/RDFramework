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

    // point
    MatFeature pnt = MatFeature::ones(1, 1);
    QString pnt_str = "  ---  \n"
                      "|  -  |\n"
                      "  ---  \n";


    // edges
    MatFeature edge = MatFeature::ones(6, 6);
    edge(cv::Range(edge.rows / 2, edge.rows), cv::Range::all()) = -1;
    QString edge_str = "  ---   ---   --- \n"
                       "|  -  |  -  |  -  | \n"
                       "  ---   ---   --- \n"
                       "| + | + | + | \n"
                       "  ---   ---   --- \n";

    QString edge_str_t = "  ---   --- \n"
                         "|  -  | + | \n"
                         "  ---   --- \n"
                         "|  -  | + | \n"
                         "  ---   --- \n"
                         "|  -  | + | \n"
                         "  ---   --- \n";



    // lines
    MatFeature line = MatFeature::ones(6, 6);
    line(cv::Range(line.rows / 3, line.rows / 2), cv::Range::all()) = -1;
    QString line_str = "  ---   ---   --- \n"
                       "| + | + | + | \n"
                       "  ---   ---   --- \n"
                       "|  -  |  -  |  -  | \n"
                       "  ---   ---   --- \n"
                       "| + | + | + | \n"
                       "  ---   ---   --- \n";


    QString line_str_t = "  ---   ---   --- \n"
                         "| + |  -  | + | \n"
                         "  ---   ---   --- \n"
                         "| + |  -  | + | \n"
                         "  ---   ---   --- \n"
                         "| + |  -  | + | \n"
                         "  ---   ---   --- \n";




    // four-rectangle
    MatFeature rect = MatFeature::ones(6, 6);
    rect(cv::Range(0, rect.rows / 2), cv::Range(rect.cols / 2, rect.cols)) = -1;
    rect(cv::Range(rect.rows / 2, rect.rows), cv::Range(0, rect.cols / 2)) = -1;
    QString rect_str = " ---  ---   ---   --- \n"
                       "| + | + | - | - | \n"
                       " ---  ---   ---   --- \n"
                       "| + | + | - | - | \n"
                       " ---  ---  ---  --- \n"
                       "| - | - | + | + | \n"
                       "  ---   ---  ---  --- \n"
                       "| - | - | + |  + |\n"
                       "  ---   ---  ---  --- \n";

    max_w = 6;
    max_h = 6;

    features.push_back(pnt);
    features_str.push_back(pnt_str);

    features.push_back(edge);
    features_str.push_back(edge_str);

    features.push_back(edge.t());
    features_str.push_back(edge_str_t);

    features.push_back(line);
    features_str.push_back(line_str);

    features.push_back(line.t());
    features_str.push_back(line_str_t);

    features.push_back(rect);
    features_str.push_back(rect_str);
}
