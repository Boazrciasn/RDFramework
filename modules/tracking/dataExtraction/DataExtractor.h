#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include "modules/tracking/videoplayer/VideoProcess.h"

template <typename BSubtractor, typename PFinder>
class DataExtractor : public VideoProcess
{
  public:
    DataExtractor(const BSubtractor &bsubtract, const PFinder &pfinder) :
        m_bsubtractor(bsubtract), m_pfinder(pfinder)
    {
    }

    cv::Mat exec() { /*Functionality here with m_img*/ }

  private:
    const BSubtractor &m_bsubtractor;
    const PFinder &m_pfinder;
};

#endif // DATAEXTRACTOR_H
