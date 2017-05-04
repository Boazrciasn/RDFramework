#ifndef PARTICLEFILTER_H
#define PARTICLEFILTER_H

#include "modules/tracking/videoplayer/VideoProcess.h"
#include "rdf/RDFBasic.h"

#include "opencv2/cudalegacy.hpp"
#include <opencv2/opencv.hpp>
#include "3rdparty/pcg-cpp-0.98/include/pcg_random.hpp"

class RectangleParticle;
class VideoReader;

class ParticleFilter : public VideoProcess
{
  public:
    ParticleFilter(int frameWidth, int frameHeight, int nParticles, int nIters, int particleWidth, int particleHeight);
    void exec(const cv::Mat &inputImg, cv::Mat &imgOut);
    ~ParticleFilter();

    void initializeParticles();
    void reInitialiaze();

    inline void setNumParticles(int value) { m_num_particles = value; }
    inline void setNumIters(int value) { m_num_iters = value; }
    inline void setParticleWidth(int value) { m_particle_width = value; }
    inline void setParticleHeight(int value) { m_particle_height = value; }
    inline void setParticlesToDisplay(int value) { m_num_particles_to_display = value; }
    inline void setRDF(RDFBasic* rdf){m_forest = rdf;}
    void processImage();
    void showParticles();
    void showTopNParticles(int count);

  private:
    void distortParticle(RectangleParticle& p);

    int m_num_particles;
    int m_num_iters;
    int m_particle_width;
    int m_particle_height;
    int m_distortRange;
    int m_num_particles_to_display;
    int img_height, img_width;
    int m_xRange;
    int m_yRange;
    QVector<RectangleParticle> m_particles;
    QVector<RectangleParticle> m_particlesNew;
    RDFBasic *m_forest{};
    pcg32 m_generator;
    std::uniform_real_distribution<> m_rand_dice;
    std::uniform_real_distribution<> m_rand_distortion;
    cv::Ptr<cv::cuda::BackgroundSubtractorGMG> m_bsMoG;
};
#endif

