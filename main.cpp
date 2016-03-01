#include "include/mainwindow.h"
#include "QApplication"
#include <QDesktopWidget>

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    MainWindow w;

    QDesktopWidget dw;
    int x=dw.width()*0.3;
    int y=dw.height()*0.9;
    w.setFixedSize(x,y);

    w.show();
    return app.exec();
}
