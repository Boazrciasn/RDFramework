#ifndef PARTICLEFILTER_H
#define PARTICLEFILTER_H

#include "modules/tracking/videoplayer/VideoProcess.h"
#include "rdf/RDFBasic.h"

#include "opencv2/cudalegacy.hpp"
#include <opencv2/opencv.hpp>
#include "3rdparty/pcg-cpp-0.98/include/pcg_random.hpp"

#include "RectangleParticle.h"
#include "Util.h"
#include "DBSCAN.h"
#include <random>
#include <math.h>

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
    int m_nTrackers{};
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

    QVector<cv::Rect> m_tracked_clusters;
    cv::Mat m_integralMat;

    RDFBasic *m_forest{};
    pcg32 m_generator;
    std::uniform_real_distribution<> m_rand_dice{};
    std::normal_distribution<> m_rand_distortion{};

    void resample(QVector<RectangleParticle>& particles, quint8 updateOrNot);

    void inline computeWeights(QVector<RectangleParticle>& particles)
    {
        for (auto& p : particles)
            p.exec(m_integralMat);
    }

    void inline normalizeWeights(QVector<RectangleParticle>& particles)
    {
        auto total_weight = sumall(particles, [](RectangleParticle P) { return P.weight; });
        for (auto& p : particles)
            p.weight = p.weight / total_weight;
    }

    int inline resamplingWheel(QVector<RectangleParticle>& particles, float max_weight)
    {
        auto num_particles = particles.size();
        auto index = (int)(m_rand_dice(m_generator)*num_particles);
        auto beta = m_rand_dice(m_generator)*2.0f*max_weight;
        while(beta > particles[index].weight)
        {
            beta -= particles[index].weight;
            index = (index + 1) % num_particles;
        }
        return index;
    }

    void inline distortParticle(RectangleParticle& p, quint8 updateOrNot)
    {
        quint16 new_x = (quint16)(p.x + p.r*cos(p.theta*M_PI/180));
        quint16 new_y = (quint16)(p.y + p.r*sin(p.theta*M_PI/180));


        p.r += m_rand_distortion(m_generator)*3;
        p.theta += m_rand_distortion(m_generator)*10;     // random [-20, 20]

        auto sig = 1.5;
        quint16 new_w = (quint16)(p.width + m_rand_distortion(m_generator)*sig);
        quint16 new_h = (quint16)(p.height + m_rand_distortion(m_generator)*sig);
        if (new_x < (img_width - new_w) && new_x > 0)
        {
            p.x = new_x;
            p.width = new_w;
            p.dx += m_rand_distortion(m_generator)*1*updateOrNot;
        }
        if (new_y < (img_height - new_h) && new_y > 0)
        {
            p.y = new_y;
            p.height = new_h;
            p.dy += m_rand_distortion(m_generator)*5*updateOrNot;
        }
    }

    void inline distortNewParticle(RectangleParticle& p)
    {
        p.r = m_rand_distortion(m_generator)*5;
        p.theta = m_rand_dice(m_generator)*360;     // random [0, 360]

        quint16 new_x = (quint16)(p.x + m_rand_distortion(m_generator));
        quint16 new_y = (quint16)(p.y + m_rand_distortion(m_generator));

        auto sig = 0.5;
        quint16 new_w = (quint16)(p.width + m_rand_distortion(m_generator)*sig);
        quint16 new_h = (quint16)(p.height + m_rand_distortion(m_generator)*sig);
        if (new_x < (img_width - new_w) && new_x > 0)
        {
            p.x = new_x;
            p.width = new_w;
        }
        if (new_y < (img_height - new_h) && new_y > 0)
        {
            p.y = new_y;
            p.height = new_h;
        }
    }

    cv::Mat m_tmp_debug;

    void inline addNewTrackers()
    {
        auto all_clusters = DBSCAN::getClusters_(m_search_particles, m_dbscan_eps, m_dbscan_min_pts);
        for(auto observed_c : all_clusters)
        {
            auto isExist = false;
            for(auto tracked_c : m_tracked_clusters)
                if(DBSCAN::normL2Sqr(observed_c,tracked_c) <= (m_dbscan_eps)*(m_dbscan_eps))
                {
                    isExist = true;
                    break;
                }
            if(!isExist)
            {
                m_tracked_clusters.push_back(observed_c);
                QVector<RectangleParticle> newTrackers(m_nTrackers);
                for(auto& p : newTrackers)
                {
                    p.x = observed_c.x;
                    p.y = observed_c.y;
                    p.width = observed_c.width;
                    p.height = observed_c.height;
                    distortNewParticle(p);
                    p.exec(m_integralMat);
                }
                m_tracking_particles.append(newTrackers);
            }
        }
    }

    void inline showRects(cv::Mat& img, QVector<cv::Rect> detect, cv::Scalar color, int th)
    {
        for(auto cluster : detect)
            cv::rectangle(img, cluster, color, th);
    }
};
#endif

