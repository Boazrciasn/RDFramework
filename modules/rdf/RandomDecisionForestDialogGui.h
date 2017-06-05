#ifndef RDFDIALOGGUI_H
#define RDFDIALOGGUI_H

#include <QDialog>
#include <QSplitter>

#include "RandomDecisionForest.h"
#include "util/ReaderGUI.h"
#include "Core/DisplayGUI.h"
#include "Core/PreProcessGUI.h"
#include "SignalSenderInterface.h"
#include "RDFBasic.h"

namespace Ui
{
class RandomDecisionForestDialogGui;
}

class RandomDecisionForestDialogGui : public QDialog
{
    Q_OBJECT
private:
    Ui::RandomDecisionForestDialogGui *ui;
    RandomDecisionForest m_forest;
    RandomDecisionForest m_rdf;
    RDFBasic m_rdf_c;
    RDFParams PARAMS;
    ReaderGUI *m_dataReaderGUI;
    DisplayGUI *m_displayImagesGUI;
    PreProcessGUI *m_preprocessGUI;
    QSplitter *m_splitterVert{};
    QSplitter *m_splitterHori{};
    quint32 m_nTreesForDetection;
    bool m_isTestDataProcessed{};
    //Pre processses in order :
    std::vector<Process> m_preprocesses;


protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    void initParamValues();
    
public:
    explicit RandomDecisionForestDialogGui(QWidget *parent = 0);
    ~RandomDecisionForestDialogGui();


public slots:
    void onImagesLoaded(bool);
    void onLabelsLoaded();
    void onTrain();
    void onTest();
    void onPreProcess();
    void printMsg(QString msg);

    void onLoad();
    void onSave();
    void writeSettings();
    void readSettings();

    void onNTreesChanged(int value);
    void onNTestTreesChanged(int value);
    void onMaxDepthChanged(int value);
    void onProbDistXChanged(int value);
    void onProbDistYChanged(int value);
    void onPixelsPerImageChanged(int value);
    void onMinLeafPixelsChanged(int value);
    void onMaxIterationChanged(int value);
    void onLabelCountChanged(int value);
    void onTauRangeChanged(int value);
    void onPositiveTau(bool value);
    void onMNIST(bool value);

private:
    template <typename T>
    void test(T& rdf)
    {
        auto begin = std::chrono::high_resolution_clock::now();


        auto size = m_dataReaderGUI->DS()->images.size();
        std::vector<cv::Mat> results{};
        Colorcode colorsBag;
        std::atomic<int> posCounter(0);
        auto pxAcc = 0.0f;
        for(auto i = 0u; i <  size; ++i)
        {
            cv::Mat labels{};
            cv::Mat_<float> confs{};
            cv::Mat_<float> layered = rdf.getLayeredHist(m_dataReaderGUI->DS()->images[i]);
            rdf.getLabelAndConfMat(layered, labels, confs);

            results.push_back(m_dataReaderGUI->DS()->images[i]);
            results.push_back(labels);

//            auto color = colorsBag.colors[m_dataReaderGUI->DS()->labels[i]];
//            auto counter = 0.0f;
//            auto c = 0.0f;
//            for (int row = 0; row < labels.rows; ++row) {
//                for (int col = 0; col < labels.cols; ++col) {
//                    if(labels.at<cv::Vec3b>(row,col) == cv::Vec3b(255,255,255))
//                        continue;
//                    ++c;
//                    if (labels.at<cv::Vec3b>(row,col) == color)
//                        ++counter;
//                }
//            }

//            pxAcc += counter/c;
//            int label{};
//            float conf{};
//            rdf.detect(m_dataReaderGUI->DS()->images[i], label, conf);
//            if (label == m_dataReaderGUI->DS()->labels[i])
//                ++posCounter;
        }

        m_displayImagesGUI->setImageSet(results);
        // ///////////////////////////////////////////////////
        // ///////////////////////////////////////////////////

        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::seconds>(end-begin).count();

        printMsg("Testing time: " + QString::number(time) + QString(" sec  ") + QString::number(time/60) + QString(" min"));
        printMsg("Accuracy: " + QString::number(100 * (float) posCounter / (float)size) + "%");
        printMsg("Pixel Accuracy: " + QString::number(100 * (float) pxAcc / (float)size) + "%");

    }
};

#endif // RDFDIALOGGUI_H
