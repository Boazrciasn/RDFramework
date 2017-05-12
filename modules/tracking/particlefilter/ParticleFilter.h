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
    ParticleFilter(uint16_t frameWidth, uint16_t frameHeight, uint16_t nParticles, uint8_t nIters,
            uint16_t particleWidth, uint16_t particleHeight);
    void exec(const cv::Mat &inputImg, cv::Mat &imgOut);
    ~ParticleFilter();

    void initializeParticles();
    void reset();

    inline void setNumParticles(uint16_t value) { m_num_particles = value; }
    inline void setNumTrackers(uint16_t value) { m_nTrackers = value; }
    inline void setNumIters(uint8_t value) { m_num_iters = value; }
    inline void setParticleWidth(uint16_t value) { m_particle_width = value; }
    inline void setParticleHeight(uint16_t value) { m_particle_height = value; }
    inline void setParticlesToDisplay(uint16_t value) { m_num_particles_to_display = value; }
    inline void setDBSCANEps(float value) { m_dbscan_eps = value; }
    inline void setDBSCANMinPts(uint8_t value) { m_dbscan_min_pts = value; }
    inline void setRDF(RDFBasic* rdf){m_forest = rdf;}
    void processImage();
    void showTopNParticles(int count);

    inline void showDetections()
    {
        for(auto cluster : m_tracked_clusters)
            cv::rectangle(m_img, cluster, cv::Scalar(0, 130, 0), 2);
    }

  private:
    uint16_t m_num_particles{};
    uint16_t m_nTrackers{};
    uint8_t m_num_iters{};
    uint16_t m_particle_width{};
    uint16_t m_particle_height{};
    uint16_t m_num_particles_to_display{};
    uint16_t m_img_height{}, m_img_width{};
    uint16_t m_xRange{};
    uint16_t m_yRange{};

    float m_dbscan_eps{};
    uint8_t m_dbscan_min_pts{};

    QVector<RectangleParticle> m_search_particles;
    QVector<RectangleParticle> m_tracking_particles;
    QVector<RectangleParticle> m_particlesNew;

    QVector<cv::Rect> m_tracked_clusters;
    cv::Mat m_integralMat;

    RDFBasic *m_forest{};
    pcg32 m_generator;
    std::uniform_real_distribution<> m_rand_dice{};
    std::normal_distribution<> m_rand_distortion{};
    Colorcode colors;

    void resample(QVector<RectangleParticle>& particles, bool isSearch);

    void inline computeWeights(QVector<RectangleParticle>& particles)
    {
        for (auto& p : particles)
            p.exec(m_integralMat);
    }

    void inline normalizeWeights(QVector<RectangleParticle>& particles)
    {
        auto total_weight = sumall(particles, [](RectangleParticle P) { return P.weight; });
        for (auto& p : particles)
            p.weight = p.weight / (float)total_weight;
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

    void inline distortParticle(RectangleParticle& p)
    {
        quint16 new_x = (quint16)(p.x + p.r*cos((p.theta)*M_PI/180));
        quint16 new_y = (quint16)(p.y + p.r*sin((p.theta)*M_PI/180));


        p.r += m_rand_distortion(m_generator)*2;
        p.theta += m_rand_distortion(m_generator)*45/p.age;

        auto sig = 1.5;
        quint16 new_w = (quint16)(p.width + m_rand_distortion(m_generator)*sig);
        quint16 new_h = (quint16)(p.height + m_rand_distortion(m_generator)*sig);
        if (new_x < (m_img_width - new_w) && new_x > 0)
        {
            p.x = new_x;
            p.width = new_w;
        }
        if (new_y < (m_img_height - new_h) && new_y > 0)
        {
            p.y = new_y;
            p.height = new_h;
        }






//        p.x = (quint16)(p.x + p.r*cos((p.theta)*M_PI/180));
//        p.y = (quint16)(p.y + p.r*sin((p.theta)*M_PI/180));
//        p.r += m_rand_distortion(m_generator)*2;
//        p.theta += m_rand_distortion(m_generator)*45/p.age;

//        auto sig = 1.5;
//        p.width = (quint16)(p.width + m_rand_distortion(m_generator)*sig);
//        p.height = (quint16)(p.height + m_rand_distortion(m_generator)*sig);


//        quint16 new_x = std::min(std::max((int)p.x, 0), m_img_width-1);
//        quint16 new_y = std::min(std::max((int)p.y, 0), m_img_height-1);
//        quint16 new_w = std::min(p.x + p.width, m_img_width-1) - new_x;
//        quint16 new_h = std::min(p.y + p.height, m_img_height-1) - new_y;

//        p.x = new_x;
//        p.y = new_y;
//        p.width = new_w;
//        p.height = new_h;
    }

    void inline distortSearchParticle(RectangleParticle& p)
    {
        auto sig = 2.0f;
        quint16 new_x = (quint16)(p.x + m_rand_distortion(m_generator)*sig);
        quint16 new_y = (quint16)(p.y + m_rand_distortion(m_generator)*sig);

        sig = 1.5;
        quint16 new_w = (quint16)(p.width + m_rand_distortion(m_generator)*sig);
        quint16 new_h = (quint16)(p.height + m_rand_distortion(m_generator)*sig);
        if (new_x < (m_img_width - new_w) && new_x > 0)
        {
            p.x = new_x;
            p.width = new_w;
        }
        if (new_y < (m_img_height - new_h) && new_y > 0)
        {
            p.y = new_y;
            p.height = new_h;
        }
    }

    void inline distortNewParticle(RectangleParticle& p)
    {
        p.r = uint8_t(m_rand_distortion(m_generator)*5);
        p.theta = 90;// m_rand_dice(m_generator)*180;
        p.age = 1;

        auto new_x = uint16_t(p.x + m_rand_distortion(m_generator)*2);
        auto new_y = uint16_t(p.y + m_rand_distortion(m_generator)*2);

        auto sig = 0.5;
        quint16 new_w = (quint16)(p.width + m_rand_distortion(m_generator)*sig);
        quint16 new_h = (quint16)(p.height + m_rand_distortion(m_generator)*sig);
        if (new_x < (m_img_width - new_w) && new_x > 0)
        {
            p.x = new_x;
            p.width = new_w;
        }
        if (new_y < (m_img_height - new_h) && new_y > 0)
        {
            p.y = new_y;
            p.height = new_h;
        }
    }

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
                    p.x = uint16_t(observed_c.x);
                    p.y = uint16_t(observed_c.y);
                    p.width = uint16_t(observed_c.width);
                    p.height = uint16_t(observed_c.height);
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

    void inline showDetections(QVector<RectangleParticle>& particles, cv::Mat& img)
    {
        for(auto& p : particles)
        {
            auto rgb = colors.colors[p.cluster%colors.colors.size()];
            cv::rectangle(img, p.boundingBox(), cv::Scalar(rgb[0], rgb[1], rgb[2]), 1);
        }
    }
};
#endif

