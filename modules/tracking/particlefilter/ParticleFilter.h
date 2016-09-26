#ifndef PARTICLEFILTER_H
#define PARTICLEFILTER_H

#include <iostream>
#include <random>
#include <atomic>
#include <chrono>
#include <QMutex>
#include <QThread>
#include "ImgProcessor.h"

class Particle;
class Target;
class VideoReader;

enum class ParticleType
{
    Rectangle,
    Circle
};

class ParticleFilter : public ImgProcessor
{
  public:
    Target *m_target;

    ParticleFilter(int frameWidth, int frameHeight, int nParticles, int nIters, int particleWidth, int particleHeight, int histSize, Target *target);
    ~ParticleFilter();

    inline int getNumParticles() const { return m_num_particles; }
    inline int getHistSize() const { return m_histSize; }
    inline int getNumIters() const { return m_num_iters; }
    inline int getParticleWidth() const { return m_particle_width; }
    inline int getModelType() const { return type; }
    int getRatioOfTop(int count);
    cv::Mat getIMG() const override { return m_outIMG; }
    std::vector<Particle *> getParticles() const {return m_particles; }
    inline void setNumParticles(int value) { m_num_particles = value; }
    inline void setHistSize(int histSize) { m_histSize = histSize; }
    inline void setNumIters(int value) { m_num_iters = value; }
    inline void setTarget(Target *target) { m_target = target;}
    inline void setParticleWidth(int value) { m_particle_width = value; }
    inline void setParticleHeight(int value) { m_particle_height = value; }
    inline void setModelType(int value) { type = value; }
    inline void setParticlesToDisplay(int value) { m_num_particles_to_display = value; }
    void setVideoReader(VideoReader *videoReader);
    void setIMG(cv::Mat *pImage) override { m_img = pImage; }
    void processImage() override;
    void showParticles();
    void showTopNParticles(int count);
    void reInitialiaze();

  signals:
    void processedImage(const QImage &image);

  private:
    void initializeParticles();
    int randomParticle(float rand_number);
    void distortParticle(Particle *p, int &x, int &y);

    int m_num_particles;
    int m_num_iters;
    int m_particle_width;
    int m_particle_height;
    int m_histSize;
    int m_distortRange;
    std::atomic<int> m_num_particles_to_display;
    int type;
    std::vector<Particle *> m_particles;
    std::vector<cv::Point> m_newCoordinates;

    std::vector<Particle *> m_ParticlesNew;
    cv::Mat *m_img;
    cv::Mat m_outIMG;
    int img_height, img_width;
    std::mt19937 m_RandomGen;
    VideoReader *m_VideoReader;

};
#endif

