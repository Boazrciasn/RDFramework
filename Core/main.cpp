#include "precompiled.h"

#include <vector>

#include "Core/MainWindowGui.h"

#include <random>
#include "Util.h"
#include "rdf/Node.h"

using namespace std;

int main(int argc, char *argv[])
{
//    MatFeature edge = MatFeature::ones(6, 6);
//    edge(cv::Range(edge.rows / 2, edge.rows), cv::Range::all()) = -1;
//    std::cout << edge << std::endl;

//    RandomDecisionTree *tree = new RandomDecisionTree();
//    cout << Feature::features.size() << endl;

//    for (auto f : Feature::features)
//        cout << f << endl;



    ////////////////////////////////////////////////
    ////////////////////////////////////////////////

//    cv::namedWindow("Feature");
//    Feature::init();
//    Feature::load("/home/neko/Desktop/DesktopFolders/trackingData/features9.txt");
//    for (int var = 0; var < Feature::features_mat.rows; ++var)
//    {
//        int rect_count = Feature::features_mat(var,0);
//        cv::Mat tmp = cv::Mat::zeros(Feature::max_w,Feature::max_h,CV_8UC1);

//        for (int i = 0; i < rect_count; ++i, ++var) {

////            cv::rectangle(tmp, cv::Rect(cv::Point(Feature::features_mat(var,3), Feature::features_mat(var,4)),
////                                        cv::Point(Feature::features_mat(var,5), Feature::features_mat(var,6))),
////                                        255*Feature::features_mat(var,1));

//            cv::Point vertices[4];
//            vertices[0] = cv::Point(Feature::features_mat(var, LEFT), Feature::features_mat(var, TOP));
//            vertices[1] = cv::Point(Feature::features_mat(var, LEFT), Feature::features_mat(var, BOTTOM));
//            vertices[2] = cv::Point(Feature::features_mat(var, RIGHT), Feature::features_mat(var, BOTTOM));
//            vertices[3] = cv::Point(Feature::features_mat(var, RIGHT), Feature::features_mat(var, TOP));
//            cv::fillConvexPoly(tmp,
//                               vertices,
//                               4,
//                               255*(Feature::features_mat(var, POS_NEG_REG) + 1)/2);
//        }

//        --var;
//        cv::resize(tmp, tmp, cv::Size(), 4, 4);
//        cv::imshow("Feature", tmp);
//        cv::waitKey();

//    }
//    return 0;

    ////////////////////////////////////////////////
    ////////////////////////////////////////////////

    Feature::init();
    Feature::load("/home/neko/Desktop/DesktopFolders/trackingData/features9.txt");
    QApplication app(argc, argv);
    MainWindowGui w;
    w.show();
    return app.exec();
}
