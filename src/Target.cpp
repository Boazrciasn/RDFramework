#include "precompiled.h"
#include "Target.h"

Target::Target(QString label, QImage image) :  m_Label(label), m_TargetImg(image)
{

}

void Target::setLabel(QString label)
{
    m_Label = label;
}

void Target::setImage(QImage image)
{
    m_TargetImg = image;
}

QString Target::getLabel()
{
    return m_Label;
}

QImage Target::getImage()
{
    return m_TargetImg;
}

Target::~Target()
{

}


