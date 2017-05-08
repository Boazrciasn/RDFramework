#include "precompiled.h"

#include <vector>

#include "Core/MainWindowGui.h"

#include <random>
#include "Util.h"
#include "rdf/Node.h"

using namespace std;

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    MainWindowGui w;
    w.show();
    return app.exec();
}
