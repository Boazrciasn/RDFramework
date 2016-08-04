#include "src/util/precompiled.h"
#include "Target.h"
#include <iostream>

Target::Target(QString label, QImage image, int histSize)
{
    m_label = label;
    m_histSize = histSize;
    m_targetImg = Util::toCv(image, CV_8UC4);
    cv::cvtColor(m_targetImg, m_targetImg, CV_RGBA2RGB);
    Util::CalculateHistogram(m_targetImg, m_targetHist, m_histSize);
}

void Target::setHistSize(int histSize)
{
    m_histSize = histSize;
    Util::CalculateHistogram(m_targetImg, m_targetHist, m_histSize);
}

void Target::setLabel(QString label)
{
    m_label = label;
}

void Target::setImage(QImage image)
{
    m_targetImg = Util::toCv(image, CV_8UC4);
    cv::cvtColor(m_targetImg, m_targetImg, CV_RGBA2RGB);
    Util::CalculateHistogram(m_targetImg, m_targetHist, m_histSize);
}





Target::~Target()
{
}


