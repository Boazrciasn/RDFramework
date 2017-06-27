#include "Feature.h"

#define FEATURE_COL_SIZE 7
#define LEFT 3
#define TOP 4
#define RIGHT 5
#define BOTTOM 6

QVector<MatFeature> Feature::features;
MatFeature Feature::features_mat;
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

//    max_w = 4;
//    max_h = 4;
    max_w = 64;
    max_h = 64;

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



//    // point
//    MatFeature pt_mat = (MatFeature(1, 7) << 2, 0, 2.0f/(max_w*max_h), 0, 0, max_w, max_h/2);
//    features_mat.push_back(pt_mat);
//    features_mat.push_back(pt_mat);
//    MatFeature pt_mat_t = (MatFeature(1, 7) << 2, 0, 2.0f/(max_w*max_h), 0, 0, max_w/2, max_h);
//    features_mat.push_back(pt_mat_t);
//    features_mat.push_back(pt_mat_t);


//    // edges
//    MatFeature edges_top_mat = (MatFeature(1, 7) << 2, 1, 2.0f/(max_w*max_h), 0, 0, max_w, max_h/2);
//    features_mat.push_back(edges_top_mat);
//    MatFeature edges_bot_mat = (MatFeature(1, 7) << 2, 1, 2.0f/(max_w*max_h), 0, max_h/2, max_w, max_h);
//    features_mat.push_back(edges_bot_mat);

//    MatFeature edges_top_t_mat = (MatFeature(1, 7) << 2, 1, 2.0f/(max_w*max_h), 0, 0, max_w/2, max_h);
//    features_mat.push_back(edges_top_t_mat);
//    MatFeature edges_bot_t_mat = (MatFeature(1, 7) << 2, 1, 2.0f/(max_w*max_h), max_w/2, 0, max_w, max_h);
//    features_mat.push_back(edges_bot_t_mat);
}

void Feature::load(QString features_file)
{
    QFile in_features(features_file);
    if (!in_features.open(QIODevice::ReadOnly))
        return;

    QTextStream input(&in_features);
    auto line    = input.readLine().trimmed();
    auto total   = line.toInt();
    auto counter = 0;

    while (!input.atEnd() && counter < total)
    {
        line = input.readLine().trimmed();
        if(line.isEmpty())
            continue;
        auto tokens = line.split(" ");
        auto rect_count = tokens[tokens.size()-1].toInt();


        MatFeature new_feature = MatFeature::zeros(rect_count, FEATURE_COL_SIZE);
        new_feature(0,0) = rect_count;


        for (auto i = 0; i < rect_count; ++i) {
            line = input.readLine().trimmed();
            tokens = line.split(" ");

            new_feature(i,1) = (tokens[tokens.size()-1].toInt() + 1) / 2;

            new_feature(i, LEFT) = tokens[0].toFloat()*max_w;
            new_feature(i, TOP) = tokens[1].toFloat()*max_h;
            new_feature(i, RIGHT) = (int)(tokens[2].toFloat()*max_w);
            new_feature(i, BOTTOM) = (int)(tokens[3].toFloat()*max_h);

            new_feature(i,2) = (new_feature(i, RIGHT) - new_feature(i, LEFT))
                    * (new_feature(i, BOTTOM) - new_feature(i, TOP));                     // how many points in region

        }
        features_mat.push_back(new_feature.clone());
        new_feature.release();
        ++counter;
    }
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
