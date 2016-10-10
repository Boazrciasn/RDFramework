#include "precompiled.h"

#include "HOGExtactor.h"
#include <QFileDialog>
#include "Util.h"
#include "util/Reader.h"

HOGExtactor::HOGExtactor()
{
    QString dir = QFileDialog::getExistingDirectory(nullptr, QObject::tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    Reader::findImages(dir, m_trainDataFiles);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                extractHOG();
}

void HOGExtactor::extractHOG()
{
    doForAll<float>(m_trainDataFiles, m_trainDataDescriptors,
                    [&](std::string path)
    {
        cv::Mat grayImg = cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE);
        cv::resize(grayImg,grayImg,cv::Size(64,128));
        std::vector<cv::Point> positions;
        std::vector<float> descriptor;
        m_hog.compute(grayImg, descriptor, cv::Size(128,64), cv::Size(16,16), positions);
        return descriptor;
    });

    for (auto desc : m_trainDataDescriptors)
        m_result.push_back((cv::Mat_<float>(desc)).t());
}

HOGExtactor::~HOGExtactor()
{
    m_result.release();
    m_trainDataFiles.clear();
    m_trainDataDescriptors.clear();
}
