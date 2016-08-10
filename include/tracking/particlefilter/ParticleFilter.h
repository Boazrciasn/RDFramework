#ifndef PARTICLEFILTER_H
#define PARTICLEFILTER_H

#include <iostream>
#include <random>
#include <atomic>
#include <QMutex>
#include <QThread>

#include "Particle.h"
#include "Target.h"
#include "RectangleParticle.h"
#include "VideoReader.h"

class ParticleFilter
{

  public:
    ParticleFilter(int frameWidth, int frameHeight, int nParticles, int nIters, int particleWidth, int particleHeight, Target *target);
    Target *m_target;

    inline int getNumParticles() const { return m_num_particles; }
    inline int getHistSize() const { return m_histSize; }
    inline int getNumIters() const { return m_num_iters; }
    inline int getParticleWidth() const { return m_particle_width; }
    inline int getModelType() const { return type; }
    inline int getRatioOfTop(int count)
    {
        float total_weight = 0;
        for (int i = 0; i < count; ++i)
            total_weight += m_Particles[i]->getWeight();
        return total_weight * 100;
    }
    cv::Mat getIMG() const { return m_outIMG; }
    std::vector<Particle *> getParticles() const {return m_Particles; }
    inline void setNumParticles(int value) { m_num_particles = value; }
    inline void setHistSize(int histSize) { m_histSize = histSize; }
    inline void setNumIters(int value) { m_num_iters = value; }
    inline void setTarget(Target *target) { m_target = target;}
    inline void setParticleWidth(int value) { m_particle_width = value; }
    inline void setParticleHeight(int value) { m_particle_height = value; }
    inline void setModelType(int value) { type = value; }
    inline void setParticlesToDisplay(int value) { m_num_particles_to_display = value; }
    inline void setVideoReader(VideoReader *videoReader) {m_VideoReader = videoReader; }
    void setIMG(cv::Mat *pImage) { m_img = pImage; }
    void processImage();
    void showParticles();
    void showTopNParticles(int count);
    ~ParticleFilter();

  signals:
    void processedImage(const QImage &image);

  private:
    void initializeParticles();
    void updateWeights();
    //TODO: Lambda func practice
    void sortParticlesDescending();
    void swap(int indA, int indB);
    void normalizeWeights();
    void resampleParticles();
    int randomParticle();
    void distortParticle(Particle *p, int &x, int &y);
    void updateParticles();

    int m_num_particles;
    int m_num_iters;
    int m_particle_width;
    int m_particle_height;
    int m_histSize = 10;
    std::atomic<int> m_num_particles_to_display;
    int type = Rectangle;
    std::vector<Particle *> m_Particles;

    std::vector<cv::Point> m_newCoordinates;

    std::vector<Particle *> m_ParticlesNew;
    cv::Mat *m_img;
    cv::Mat m_outIMG;
    int img_height, img_width;
    std::default_random_engine generator;
    VideoReader *m_VideoReader;

};
#endif

