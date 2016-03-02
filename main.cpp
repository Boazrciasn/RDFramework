#include "include/mainwindow.h"
#include "QApplication"
#include "vector"
#include "QString"
#include "QDebug"
//TEST CASE
#include "include/Reader.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
