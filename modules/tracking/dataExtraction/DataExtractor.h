#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

template<typename BSubtractor, typename PFinder, typename BBCalculator>
class DataExtractor
{
  public:
    DataExtractor(BSubtractor bsubtract, PFinder pfinder, BBCalculator bbcalculator)
    {
        m_bsubtractor =  bsubtract;
        m_pfinder = pfinder;
        m_bbcalculator = bbcalculator;
    }

  private:
    BSubtractor m_bsubtractor;
    PFinder m_pfinder;
    BBCalculator m_bbcalculator;
};

#endif // DATAEXTRACTOR_H
