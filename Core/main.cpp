#include "precompiled.h"

#include <vector>
#include "Core/MainWindowGui.h"

#include <random>
#include "Util.h"
#include "cpuafiinityinterface.h"

#include <mlpack/core.hpp>
#include <mlpack/methods/mean_shift/mean_shift.hpp>
#include <armadillo>




int NCPU;
using namespace std;

int main(int argc, char *argv[])
{

    NCPU = setAffinity();
//    Mean shift Test
    arma::mat data;
    data.load("/Users/barisozcan/Downloads/data.csv", arma::csv_ascii);
    data = data.t();
    std::cout<< data.n_cols << std::endl;
    arma::Col<size_t> assignments; // Cluster assignments.
    arma::mat centroids; // Cluster centroids.
    mlpack::meanshift::MeanShift<> MeanShift;
    MeanShift.Cluster(data,assignments,centroids);
    assignments.print();

    Feature::init();
    Feature::load("/HOME/baris.ozcan/ImageCLEF/src/features9.txt");


    QApplication app(argc, argv);
    MainWindowGui w;
    w.show();
    return app.exec();
}
