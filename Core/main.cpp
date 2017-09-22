#include "precompiled.h"

#include <vector>
#include "Core/MainWindowGui.h"

#include <random>
#include "Util.h"
#include "rdf/Node.h"


using namespace std;

int main(int argc, char *argv[])
{
    const int iterationCount = 1000000000;
    double pi{};

    #pragma omp parallel for num_threads(56)
    for (int i = 0; i < iterationCount; i++)
    {
        pi += 4 * (i % 2 ? -1 : 1) / (2.0 * i + 1.0);
        qDebug()<<i;
    }

    #pragma omp parallel for num_threads(56)
    for (int i = 0; i < iterationCount; i++)
    {
        pi += 4 * (i % 2 ? -1 : 1) / (2.0 * i + 1.0);
    }
    qDebug()<<pi;
    #pragma omp parallel for num_threads(56)
    for (int i = 0; i < iterationCount; i++)
    {
        pi += 4 * (i % 2 ? -1 : 1) / (2.0 * i + 1.0);
    }
    qDebug()<<pi;
    #pragma omp parallel for num_threads(56)
    for (int i = 0; i < iterationCount; i++)
    {
        pi += 4 * (i % 2 ? -1 : 1) / (2.0 * i + 1.0);
    }
    qDebug()<<pi;

    Feature::init();
    Feature::load("/HOME/baris.ozcan/ImageCLEF/src/features9.txt");


    QApplication app(argc, argv);
    MainWindowGui w;
    w.show();
    return app.exec();
}
