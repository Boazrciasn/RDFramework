#include "include/mainwindow.h"
#include "QApplication"
#include <QDesktopWidget>
#include "include/Reader.h"
#include "QDebug"
#include "vector"


int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    MainWindow w;
//    QString dir  = "/Users/barisozcan/Documents/Computer_Vision/ImageCLEF/Samples";
//    Reader read;
//    vector<QString> vec;
//    read.findImages(dir,"x",vec);
//    for (int i = 0; i < vec.size(); ++i) {
//        qDebug()<< vec.at(i);
//    }


    QDesktopWidget dw;
    int x=dw.width()*0.3;
    int y=dw.height()*0.9;
    w.setFixedSize(x,y);

    w.show();
    return app.exec();
}
