#include "precompiled.h"

#include <vector>
#include "Core/MainWindowGui.h"

#include <random>
#include "Util.h"
#include "cpuafiinityinterface.h"

int NCPU;
using namespace std;

int main(int argc, char *argv[])
{

    NCPU = setAffinity();

    Feature::init();
    Feature::load("/HOME/baris.ozcan/ImageCLEF/src/features9.txt");


    QApplication app(argc, argv);
    MainWindowGui w;
    w.show();
    return app.exec();
}
