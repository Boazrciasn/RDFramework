#ifndef PFEXECUTOR_H
#define PFEXECUTOR_H

#include <QMutex>
#include <QThread>
#include <tuple>
#include <opencv2/video/background_segm.hpp>

#include "VideoReader.h"
#include "BufferQueue.h"
#include "modules/tracking/dataExtraction/DataExtractor.h"
#include "VideoProcess.h"

class VideoPlayer : public QThread
{
    Q_OBJECT

  signals :
    void playerFrame(const QImage &image);

  protected:
    void run();
    void msleep(int ms);

  private:

    VideoReader *m_VideoReader;
    BufferQueue<cv::Mat> *m_FrameBuffer;
    bool m_stop = true;
    QMutex m_mutex;
    QWaitCondition m_waitcond;
    cv::Mat m_frame_out;
    double m_frameRate;
    cv::Mat m_frame;
    QImage m_img;
    double m_CurrentFrame;

    int m_frame_w = 416;
    int m_frame_h = 416;

    VideoProcess *m_processor{};


//    template <typename ImgProcessor>
    void processImage();

    void initializeVideo();


    cv::Ptr<cv::BackgroundSubtractorMOG2> m_pMOG;
    cv::Mat_<uchar> m_fgMaskMOG2;
    cv::Mat m_fgMaskMOG2_dilated;
    cv::Mat m_fgMaskMOG2_noHole;

    cv::MorphShapes m_erosion_elem = cv::MORPH_RECT;
    int m_erosion_size = 4;
    cv::MorphShapes m_dilation_elem = cv::MORPH_RECT;
    int m_dilation_size = 4;

    /** Function Headers */
    void Erosion(int, void *);
    void Dilation(int, void *);

  public:
    VideoPlayer(QObject *parent);
    ~VideoPlayer();

    bool loadVideo(std::string filename);
    void playVideo();
    void stopVideo() { m_stop = true; }
    bool isStopped() const { return m_stop; }
    double getCurrentFrame();
    inline VideoProcess *getVideoProcess() const { return m_processor; }

    int getNumberOfFrames() const { return m_VideoReader->getNumberOfFrames(); }
    std::tuple<int, int> getFrameSize();


    inline void setProcess(VideoProcess *process) { m_processor = process; }
    void setCurrentFrame(int frameNumber);
};

#endif // PFEXECUTOR_H
