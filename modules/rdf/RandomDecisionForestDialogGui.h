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
#include "util/MeanShift.h"

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

            auto color = colorsBag.colors[m_dataReaderGUI->DS()->labels[i]];
            auto counter = 0.0f;
            auto c = 0.0f;
            for (int row = 0; row < labels.rows; ++row) {
                for (int col = 0; col < labels.cols; ++col) {
                    if(labels.at<cv::Vec3b>(row,col) == cv::Vec3b(255,255,255))
                        continue;
                    ++c;
                    if (labels.at<cv::Vec3b>(row,col) == color)
                        ++counter;
                }
            }

            pxAcc += counter/c;
            int label{};
            float conf{};
            rdf.detect(m_dataReaderGUI->DS()->images[i], label, conf);
            if (label == m_dataReaderGUI->DS()->labels[i])
                ++posCounter;
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

    template <typename T>
    void testSingleFrame(T& rdf)
    {
        auto begin = std::chrono::high_resolution_clock::now();


        auto size = m_dataReaderGUI->DS()->images.size();
        std::vector<cv::Mat> results{};
        Colorcode colorsBag;
        std::atomic<int> posCounter(0);
        auto pxAcc = 0.0f;
        for(auto i = 0u; i <  50; ++i)
        {
            qDebug() << "Image " << i << " is being procecced!";
            cv::Mat labels{};
            cv::Mat_<float> confs{};
            auto layered = rdf.getLayeredHist(m_dataReaderGUI->DS()->images[i]);
            auto rects = m_dataReaderGUI->DS()->frameRects[i];
            rdf.getLabelAndConfMat(layered, labels, confs);





            auto& img = m_dataReaderGUI->DS()->images[i];
            cv::Mat_<uchar> regression = cv::Mat_<uchar>::zeros(img.rows, img.cols);
            cv::Mat_<uchar> regressionW = cv::Mat_<uchar>::zeros(img.rows, img.cols);
            rdf.getRegressionResult(img, regression, regressionW);


            /////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////

            MeanShift *msp = new MeanShift();
            double kernel_bandwidth = 11;
            std::vector<std::vector<double> > points;
            qDebug()<< "Regression started!";

            for (int r = 0; r < regression.rows; ++r) {
                for (int c = 0; c < regression.cols; ++c) {
                    if(regression(r,c) > 5)
                    {
                        std::vector<double> pt;
                        pt.push_back(c);
                        pt.push_back(r);
                        points.push_back(pt);

                        cv::Point center(c,r);
                        cv::Point halfWH(regressionW(r,c), 2.5*regressionW(r,c));
                        cv::circle(labels, center, 2, cv::Scalar(255,255,255), 1);
//                        cv::rectangle(img, cv::Rect(center-halfWH, center+halfWH), cv::Scalar(255,0,0));
                    }
                }
            }

            auto clusters = msp->cluster(points, kernel_bandwidth);


            // K-Means
//            auto clusterCount = 3;
//            cv::Mat_<float> pts(points.size(), 2);
//            cv::Mat lbls, centers;
//            for(auto pt = 0; pt < points.size(); ++pt)
//            {
//                pts(pt,0) = points[pt][0];
//                pts(pt,1) = points[pt][1];
//            }

//            cv::kmeans(pts, clusterCount, lbls,
//                        cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 10, 1.0),
//            3, cv::KMEANS_PP_CENTERS, centers);

//            for (int var = 0; var < centers.rows; ++var) {
//                cv::Point center(centers.at<float>(var, 0), centers.at<float>(var, 1));
//                cv::circle(img, center, 10, cv::Scalar(255,0,0), 2);
//            }


            for(auto cluster : clusters)
            {
                if(cluster.shifted_points.size() < 5)
                    continue;
                cv::Point center(cluster.shifted_points[0][0], cluster.shifted_points[0][1]);
                cv::Point halfWH(20,40);
                cv::rectangle(img, cv::Rect(center-halfWH, center+halfWH), cv::Scalar(255,0,0), 2);
                cv::circle(labels, center, 5, cv::Scalar(0,255,0), 2);
            }


            /////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////


//            for (int r = 0; r < regression.rows; ++r) {
//                for (int c = 0; c < regression.cols; ++c) {
//                    if(regression(r,c) > 5)
//                    {
//                        cv::Point center(c,r);
//                        cv::Point halfWH(regressionW(r,c), 2.5*regressionW(r,c));
//                        cv::circle(labels, center, 2, cv::Scalar(255,255,255), 1);
//                        cv::rectangle(img, cv::Rect(center-halfWH, center+halfWH), cv::Scalar(255,0,0));
//                    }
//                }
//            }


            results.push_back(img.clone());
            results.push_back(labels.clone());
//            results.push_back(reg);




            auto color_pos = colorsBag.colors[0];       // TODO:: in single frame 0 is positive and 1 is negative
            auto color_neg = colorsBag.colors[1];

            auto gt = labels.clone();
            gt.setTo(color_neg);

            // Positive values
            for (QRect rect : rects) {
                gt(cv::Rect(rect.x(), rect.y(), rect.width(), rect.height())) = color_pos;
            }




            auto counter = 0.0f;
            for (int row = 0; row < labels.rows; ++row)
                for (int col = 0; col < labels.cols; ++col)
                    if (labels.at<cv::Vec3b>(row,col) == gt.at<cv::Vec3b>(row,col))
                        ++counter;

            pxAcc += counter/(labels.rows*labels.cols);

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
