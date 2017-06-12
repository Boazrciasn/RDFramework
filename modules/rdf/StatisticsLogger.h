#ifndef STATISTICSLOGGER_H
#define STATISTICSLOGGER_H
#include <QtCore/QtCore>
#include "SignalSenderInterface.h"
#include "tbb/concurrent_vector.h"
#include "Feature.h"

class StatisticsLogger
{
  public:
    StatisticsLogger();
    void inline logPxCount(quint32 pxs, quint8 d)
    {
        m_pxCount[d] = pxs;
        if (d == 0)
            m_pxRatio[d] = 100.0;
        else
            m_pxRatio[d] = 100 * ((double)pxs / m_pxCount[0]);
    }
    void inline logLeafCount(quint32 leaves, quint8 d) { m_leafCount[d] = leaves; }
    void inline logImpurity(quint32 impurity, quint8 d)
    {
        m_impurity[d] = impurity;
        if (d == 0)
            m_impurityRatio[d] = 100.0;
        else
            m_impurityRatio[d] = 100 * ((double)impurity / m_impurity[0]);
    }
    void inline setDepth(int d)
    {
        m_pxCount.resize(d);
        m_leafCount.resize(d);
        m_pxRatio.resize(d);
        m_impurity.resize(d);
        m_impurityRatio.resize(d);
    }

    void inline logFeatureFreq(tbb::concurrent_vector<int> frequency) {
        m_featureSelectionRatio.resize((int)frequency.size());
        auto total = std::accumulate(frequency.begin(), frequency.end(), 0);
        for (auto i = 0u; i < frequency.size(); ++i) {
            m_featureSelectionRatio[i] = 100*frequency[i]/total;
        }
    }

    void inline logTrainingTime(double time) {m_trainingTime  = time;}

    void dump()
    {
        SignalSenderInterface::instance().printsend(m_header);
        for (int d = 0; d < m_pxCount.size(); ++d)
            SignalSenderInterface::instance().printsend
                    (QString::number(d) + "  " + QString::number(
                         m_pxCount[d]) + "  " + QString::number(
                         m_pxRatio[d]) + "%  " + QString::number(m_leafCount[d]) + "  " + QString::number(m_impurity[d]) + "  " +
                     QString::number(m_impurityRatio[d]) + "%");

        SignalSenderInterface::instance().printsend("Training Time : " + QString::number(m_trainingTime) + QString(" sec  /  ")
                                                    + QString::number(m_trainingTime/60) + QString(" min"));

        dumpFeatureFrequency();
    }

    void dumpFeatureFrequency()
    {
        QString features;
        for (int d = 0; d < m_featureSelectionRatio.size(); ++d)
            features += QString::number(d).sprintf(": %2.2f %%\n", m_featureSelectionRatio[d]) + Feature::features_str[d];



        SignalSenderInterface::instance().printsend(features);
    }

  private:
    QVector<quint32> m_pxCount;
    QVector<quint32> m_leafCount;
    QVector<double> m_pxRatio;
    QVector<quint32> m_impurity;
    QVector<double> m_impurityRatio;
    QVector<double> m_featureSelectionRatio;
    double m_trainingTime;
    QString m_header = "Depth " + QString(" Pixels ") + QString(" PxRatio ") + QString(" LCount ") + QString(" Impurity ") +
                       QString(" ImpRat ");
};

#endif // STATISTICSLOGGER_H
