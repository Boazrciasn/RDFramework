#include "src/precompiled.h"

#include "include/mainwindow.h"
#include "include/Reader.h"
#include "include/RandomDecisionForest.h"

int main(int argc, char *argv[]) {


//    QString file = "/home/neko/Desktop/lastSession.txt";
//    QFile  *m_saveFile = new QFile(file);
//    bool fileExists = m_saveFile->exists();
//    m_saveFile->open(QIODevice::ReadWrite);

//    if (fileExists)
//    {
//        // Read header and set values
//        qDebug()<<"file does not exist! " << fileExists;
//        int c = 0;
//        do
//        {
//            qDebug()<< m_saveFile->readLine();
//            c++;
//        }while(m_saveFile->canReadLine() & (c < 4));

//        m_saveFile->write("Testing");
//        m_saveFile->write(" \n");
//    }
//    else
//    {
//        qDebug()<<"open success!";
//        // set initial values and create header
//        int m_avgWidth = 1;
//        int m_avgHight = 2;
//        int m_sampleCount = 100;

//        m_saveFile->write(QByteArray::number(m_avgWidth));
//        m_saveFile->write(" \n");
//        m_saveFile->write(QByteArray::number(m_avgHight));
//        m_saveFile->write(" \n");
//        m_saveFile->write(QByteArray::number(m_sampleCount));
//        m_saveFile->write(" \n");
//        m_saveFile->write(file.toUtf8());
//        m_saveFile->write(" \n");
//    }

//    m_saveFile->close();


    QApplication app(argc, argv);
    MainWindow w;

    QDesktopWidget dw;
    int xx=dw.width()*0.3;
    int yy=dw.height()*0.9;
    w.setFixedSize(xx,yy);

    w.show();
    return app.exec();
}
