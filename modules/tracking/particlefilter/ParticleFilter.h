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

    void inline initializeParticles() {
        m_search_particles.clear();
        m_search_particles.resize(m_num_particles);
        for (auto &p : m_search_particles) {
            p.x = uint16_t(m_rand_dice(m_generator) * m_xRange);
            p.y = uint16_t(m_rand_dice(m_generator) * m_yRange);
            p.width = m_particle_width;
            p.height = m_particle_height;
        }
    }
    void inline reset() {
        // TODO: add other reset things
        m_tracking_particles.clear();
    }

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
        for(auto& p : m_tracked_clusters)
            cv::rectangle(m_img, p.boundingBox(), cv::Scalar(0, 130, 0), 2);
    }

  private:
    uint16_t m_num_particles{};
    uint16_t m_nTrackers{};
    uint8_t m_num_iters{};
    uint16_t m_particle_width{};
    uint16_t m_particle_height{};
    uint16_t m_num_particles_to_display{};
    uint16_t m_img_width{}, m_img_height{};
    uint16_t m_xRange{};
    uint16_t m_yRange{};

    float m_dbscan_eps{};
    uint8_t m_dbscan_min_pts{};

    QVector<RectangleParticle> m_search_particles;
    QVector<RectangleParticle> m_tracking_particles;
    QVector<RectangleParticle> m_particlesNew;

    QVector<RectangleParticle> m_tracked_clusters;
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
        auto new_x = uint16_t(p.x + p.dx);
        auto new_y = uint16_t(p.y + p.dy);
        p.dx += m_rand_distortion(m_generator)*p.sig_x;
        p.dy += m_rand_distortion(m_generator)*p.sig_y;


        quint16 new_w = (quint16)(p.width + m_rand_distortion(m_generator)*p.sig_w);
        quint16 new_h = (quint16)(p.height + m_rand_distortion(m_generator)*p.sig_h);
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
        auto sig = 1.5f;
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
        p.r = uint8_t(m_rand_dice(m_generator)*10);
        p.theta = m_rand_dice(m_generator)*360;
        p.age = 1;

//        auto new_x = uint16_t(p.x + m_rand_distortion(m_generator)*2);
//        auto new_y = uint16_t(p.y + m_rand_distortion(m_generator)*2);

        p.dx = m_rand_distortion(m_generator)*4;
        p.dy = m_rand_distortion(m_generator)*4;
        auto new_x = uint16_t(p.x + p.dx);
        auto new_y = uint16_t(p.y + p.dy);

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

    inline float normL2Sqr(cv::Rect A, cv::Rect B){
        cv::Point cA(A.x + A.width/2, A.y + A.height/2);    // center A
        cv::Point cB(B.x + B.width/2, B.y + B.height/2);    // center B
        return DBSCAN::normL2Sqr(cA, cB);
    }

    void inline addNewTrackers()
    {
        auto eps = 17.0f;
        auto min_pts = 25;

        std::sort(std::begin(m_search_particles), std::end(m_search_particles),
                  [](RectangleParticle P1, RectangleParticle P2) {
            return P1.weight > P2.weight;
        });
        m_search_particles.resize(400);
        auto all_clusters = DBSCAN::getClusters(m_search_particles, eps, min_pts);

//        showDetections(m_search_particles, m_img);
//        showDetections(m_tracking_particles, m_img);
        showRects(m_img, all_clusters, cv::Scalar(0, 130, 0), 2);

        for(auto& observed_c : all_clusters)
        {
            auto isExist = false;
            for(auto& tracked_c : m_tracked_clusters)
            {
                eps =( std::max(tracked_c.width, tracked_c.height));
                if(DBSCAN::normL2Sqr(observed_c.center(),tracked_c.center()) <= (eps)*(eps))
                {
                    isExist = true;
                    break;
                }
            }
            if(!isExist)
            {
                observed_c.exec(m_integralMat);
                if(observed_c.weight < 140)
                    continue;
                m_tracked_clusters.push_back(observed_c);
                QVector<RectangleParticle> newTrackers(m_nTrackers);
                for(auto& p : newTrackers)
                {
                    p = observed_c.clone();
                    distortNewParticle(p);
                }
                m_tracking_particles.append(newTrackers);
            }
        }
    }

    void inline showRects(cv::Mat& img, QVector<RectangleParticle> detect, cv::Scalar color, int th)
    {
        for(auto cluster : detect)
        {
//            qDebug()<< cluster.x << " " << cluster.y << " " << cluster.r << " " << cluster.theta;
//            auto x = (quint16)(cluster.x + cluster.r*cos((cluster.theta)*M_PI/180));
//            auto y = (quint16)(cluster.y + cluster.r*sin((cluster.theta)*M_PI/180));

            auto x = uint16_t(cluster.x + 3*cluster.dx);
            auto y = uint16_t(cluster.y + 3*cluster.dy);

//            qDebug()<< x << " " << y;


            cv::Rect box = cluster.boundingBox();
            cv::rectangle(img, box, color, th);
//            cv::arrowedLine(img, box.tl(), cv::Point(x,y), cv::Scalar(200,0,0), 2);
/*            std::string data = "W: " + std::to_string(cluster.weight);
            cv::putText(img, data, cv::Point(cluster.x,cluster.y - 5),
                cv::FONT_HERSHEY_COMPLEX_SMALL, 0.5, cv::Scalar(200,200,250), 1, CV_AA)*/;
        }
    }

    void inline showDetections(QVector<RectangleParticle>& particles, cv::Mat& img)
    {
        for(auto& p : particles)
        {
            if(p.cluster == 0)
                continue;
            auto rgb = colors.colors[p.cluster%colors.colors.size()];
            cv::rectangle(img, p.boundingBox(), cv::Scalar(rgb[0], rgb[1], rgb[2]), 1);
        }
    }
    void setSTDforTrackers();

    void inline updateTrackers()
    {
        //    for (int i = 0; i < 2; ++i)
        {
            resample(m_tracking_particles, false);
            updateWeights(m_tracking_particles);
        }
        m_tracked_clusters = DBSCAN::getClusters(m_tracking_particles, m_dbscan_eps, m_dbscan_min_pts);
    }

    void inline detectObjects()
    {
        for (int i = 0; i < m_num_iters; ++i)
        {
            updateWeights(m_search_particles);
            resample(m_search_particles, true);
        }
        updateWeights(m_search_particles);
    }

    void inline updateWeights(QVector<RectangleParticle>& particles)
    {
        computeWeights(particles);
        normalizeWeights(particles);
    }
};
#endif

