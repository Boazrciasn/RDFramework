#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

template <typename BSubtractor, typename PFinder, typename BBCalculator>
class DataExtractor
{
  public:
    DataExtractor(const BSubtractor &bsubtract, const PFinder &pfinder, const BBCalculator &bbcalculator) :
        m_bsubtractor(bsubtract), m_pfinder(pfinder), m_bbcalculator(bbcalculator)
    {
    }

  private:
    const BSubtractor &m_bsubtractor;
    const PFinder &m_pfinder;
    const BBCalculator &m_bbcalculator;
};

#endif // DATAEXTRACTOR_H
