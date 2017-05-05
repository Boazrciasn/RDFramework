#ifndef PARTICLEFILTER_H
#define PARTICLEFILTER_H

#include "modules/tracking/videoplayer/VideoProcess.h"
#include "rdf/RDFBasic.h"

#include "opencv2/cudalegacy.hpp"
#include <opencv2/opencv.hpp>
#include "3rdparty/pcg-cpp-0.98/include/pcg_random.hpp"

#include "RectangleParticle.h"
#include "Util.h"

class VideoReader;

class ParticleFilter : public VideoProcess
{
  public:
    ParticleFilter(int frameWidth, int frameHeight, int nParticles, int nIters, int particleWidth, int particleHeight);
    void exec(const cv::Mat &inputImg, cv::Mat &imgOut);
    ~ParticleFilter();

    void initializeParticles();
    void reset();

    inline void setNumParticles(int value) { m_num_particles = value; }
    inline void setNumIters(int value) { m_num_iters = value; }
    inline void setParticleWidth(int value) { m_particle_width = value; }
    inline void setParticleHeight(int value) { m_particle_height = value; }
    inline void setParticlesToDisplay(int value) { m_num_particles_to_display = value; }
    inline void setDBSCANEps(float value) { m_dbscan_eps = value; }
    inline void setDBSCANMinPts(int value) { m_dbscan_min_pts = value; }
    inline void setRDF(RDFBasic* rdf){m_forest = rdf;}
    void processImage();
    void showTopNParticles(int count);
    void showDetections();

  private:
    int m_num_particles{};
    int m_num_iters{};
    int m_particle_width{};
    int m_particle_height{};
    int m_distortRange{};
    int m_num_particles_to_display{};
    int img_height{}, img_width{};
    int m_xRange{};
    int m_yRange{};

    float m_dbscan_eps{};
    int m_dbscan_min_pts{};

    QVector<RectangleParticle> m_search_particles;
    QVector<RectangleParticle> m_tracking_particles;
    QVector<RectangleParticle> m_particlesNew;

    RDFBasic *m_forest{};
    pcg32 m_generator;
    std::uniform_real_distribution<> m_rand_dice{};
    std::normal_distribution<> m_rand_distortion{};

    void resample(QVector<RectangleParticle>& particles);

    void inline computeWeights(QVector<RectangleParticle>& particles)
    {
        for (auto& p : particles)
            p.exec(m_img);
    }

    void inline normalizeWeights(QVector<RectangleParticle>& particles)
    {
        auto total_weight = sumall(particles, [](RectangleParticle P) { return P.weight(); });
        for (auto& p : particles)
            p.setWeight(p.weight() / total_weight);
    }

    int inline resamplingWheel(QVector<RectangleParticle>& particles, float max_weight)
    {
        auto num_particles = particles.size();
        auto index = (int)(m_rand_dice(m_generator)*num_particles);
        auto beta = m_rand_dice(m_generator)*2.0f*max_weight;
        while(beta > particles[index].weight())
        {
            beta -= particles[index].weight();
            index = (index + 1) % num_particles;
        }
        return index;
    }

    void inline distortParticle(RectangleParticle& p)
    {
        int new_x = p.x() + (int)(m_rand_distortion(m_generator)*m_distortRange);
        int new_y = p.y() + (int)(m_rand_distortion(m_generator)*m_distortRange);

        quint16 new_w = p.getWidth(); // + (int)(m_rand_distortion(m_generator)*m_distortRange);
        quint16 new_h = p.getHeight(); // + (int)(m_rand_distortion(m_generator)*m_distortRange);
        if (new_x < (img_width - new_w) && new_x > 0)
        {
            p.setX(new_x);
            p.setWidth(new_w);
        }
        if (new_y < (img_height - new_h) && new_y > 0)
        {
            p.setY(new_y);
            p.setHeight(new_h);
        }
    }
};
#endif

