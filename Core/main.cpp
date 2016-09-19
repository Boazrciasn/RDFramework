#include "precompiled.h"

#include <vector>

#include "Core/MainWindowGui.h"
#include "ocr/Reader.h"
#include "rdf/RandomDecisionForest.h"


#include "modules/tracking/dataExtraction/HOGExtactor.h"

int main(int argc, char *argv[])
{
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
//     MNIST EXTRACTOR :
//        QString MNIST_path = "/Users/barisozcan/Documents/MNIST_dataset/";
//        QString destdir = "/Users/barisozcan/";
//        MNIST mnist(MNIST_path);
//        mnist.MNISTReader();
//        qDebug() << "no of test images :" << mnist.m_testImagesVector->size();
//        qDebug() << "no of train images :" << mnist.m_trainImagesVector->size();
//        qDebug() << "test label count : " << mnist.m_testLabels->size();
//        qDebug() << "train label count : " << mnist.m_trainLabels->size();
//        mnist.extractDataSet(destdir);
    QApplication app(argc, argv);
    MainWindowGui w;
    w.show();

    HOGExtactor *hogExtr = new HOGExtactor();
    std::cout << hogExtr->getDataSize() << std::endl;

    return app.exec();
}
