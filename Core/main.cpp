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

    QApplication app(argc, argv);
    MainWindowGui w;
    w.show();
    return app.exec();
}
