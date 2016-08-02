#include "precompiled.h"
#include "Target.h"
#include <iostream>

Target::Target(QString label, QImage image) :  m_Label(label)
{
    m_TargetImg = Util::toCv(image, CV_8UC4);
    cv::cvtColor(m_TargetImg, m_TargetImg, CV_RGBA2RGB);
    Util::CalculateHistogram(m_TargetImg, m_TargetHist, 64);
}

void Target::setLabel(QString label)
{
    m_Label = label;
}

void Target::setImage(QImage image)
{
    m_TargetImg = Util::toCv(image, CV_8UC4);
    cv::cvtColor(m_TargetImg, m_TargetImg, CV_RGBA2RGB);
}

QString Target::getLabel()
{
    return m_Label;
}

cv::Mat Target::getImage()
{
    return m_TargetImg;
}

Target::~Target()
{
}


