#ifndef HOGEXTACTOR_H
#define HOGEXTACTOR_H

#include <QObject>
#include <QVector>
#include <QDirIterator>
#include <opencv2/objdetect.hpp>

template <typename T, typename FUNC>
void doForAll(const std::vector<QString> &v_filePath,
              QVector<std::vector<T>> &v_descriptors,
              const FUNC &func)
{
    for (auto path : v_filePath)
        v_descriptors.append(func(path.toStdString()));
}

class HOGExtactor
{
  public:
    HOGExtactor();
    ~HOGExtactor();

    inline int getDataSize() {return m_trainDataDescriptors.size();}
    inline cv::Mat_<float> getResult() {return m_result;}

  private:
    void extractHOG();

    cv::HOGDescriptor m_hog;
    cv::Mat_<float> m_result;
    std::vector<QString> m_trainDataFiles;
    QVector<std::vector<float>> m_trainDataDescriptors;
};

#endif // HOGEXTACTOR_H
