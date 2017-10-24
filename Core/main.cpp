#include "precompiled.h"

#include <vector>
#include "Core/MainWindowGui.h"

#include <random>
#include "Util.h"
#include "cpuaffinityinterface.h"

#include <mlpack/core.hpp>
#include <mlpack/methods/mean_shift/mean_shift.hpp>
#include <armadillo>




int NCPU;
using namespace std;

int main(int argc, char *argv[])
{

    NCPU = setAffinity();

//    Mean shift Test
//    arma::colvec vec({1, 2, 3, 4});
//    arma::colvec vec2({3, 4, 5, 6});
//    arma::mat votes(4,4);
//    votes.col(0) = vec;
//    votes.col(1) = vec2;
//    votes.print();
//    votes = votes.cols(0,1);
//    votes.print();
    arma::mat data;
    data.load("/Users/barisozcan/Downloads/CO2.csv", arma::csv_ascii);
    data = data.t();
    arma::Col<size_t> assignments; // Cluster assignments.
    arma::mat centroids; // Cluster centroids.
    mlpack::meanshift::MeanShift<true> MeanShift;
    MeanShift.Cluster(data,assignments,centroids);
    int max{};
    int maxcInd;
    arma::mat maxCentroid;
    for (int c = 0; c < centroids.n_cols; ++c) {
        arma::uvec ind = arma::find(assignments==c);
        std::cout<< ind.n_elem << std::endl;
        if( ind.n_elem > max)
        {
            maxCentroid = centroids.col(c);
            maxcInd = c;
            max = ind.n_elem;
        }
    }
    arma::uvec indx = arma::find(assignments==1);
    arma::mat c1 = data.cols(indx);
//    data.print();
//    indx.print();
//    std::cout<< c1.n_cols << c1.n_rows<< std::endl;
//    arma::mat covmat = arma::cov(c1.t());
//    std::cout<< covmat.n_cols << covmat.n_rows<< std::endl;

//    centroids.print();
//    assignments.print();
    Feature::init();
    Feature::load("/HOME/baris.ozcan/ImageCLEF/src/features9.txt");


    QApplication app(argc, argv);
    MainWindowGui w;
    w.show();
    return app.exec();
}
