#ifndef HOGEXTACTOR_H
#define HOGEXTACTOR_H

#include <QObject>
#include <QVector>
#include <QDirIterator>
#include <opencv2/objdetect.hpp>

template <typename T, typename FUNC>
void doForAll(const QVector<std::string> &v_filePath,
              QVector<std::vector<T>> &v_descriptors,
              const FUNC &func)
{
    for (auto path : v_filePath)
        v_descriptors.append(func(path));
}

class HOGExtactor
{
  public:
    HOGExtactor();

    inline int getDataSize() {return m_trainDataDescriptors.size();}

  private:
    void getTrainingData(QString baseDir);
    void extractHOG();

    cv::HOGDescriptor m_hog;
    QVector<std::string> m_trainDataFiles;
    QVector<std::vector<float>> m_trainDataDescriptors;
};

#endif // HOGEXTACTOR_H
