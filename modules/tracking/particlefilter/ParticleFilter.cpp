#include "precompiled.h"

#include <time.h>

#include "tracking/particlefilter/ParticleFilter.h"
#include "Target.h"
#include "tracking/particlefilter/RectangleParticle.h"
#include "DBSCAN.h"

ParticleFilter::ParticleFilter(
        int frameWidth, int frameHeight, int nParticles, int nIters,
        int particleWidth, int particleHeight) : img_height(frameHeight), img_width(frameWidth)
{
    m_num_particles_to_display = 0;
    setNumParticles(nParticles);
    setNumIters(nIters);
    setParticleWidth(particleWidth);
    setParticleHeight(particleHeight);
    setParticlesToDisplay(m_num_particles_to_display);
    m_distortRange = 1; // TODO: set from gui
    m_rand_dice    = std::uniform_real_distribution<>(0, 1);
    m_rand_distortion = std::normal_distribution<>(-1, 1);

    m_xRange = (img_width - m_particle_width);
    m_yRange = (img_height - m_particle_height);

    // Default
    setDBSCANEps(8.0f);
    setDBSCANMinPts(20);
}

void ParticleFilter::exec(const cv::Mat &inputImg, cv::Mat &imgOut)
{
    m_img = inputImg.clone();
    cv::cvtColor(m_img,m_img, CV_BGR2Lab);
    cv::Mat_<float> confs{};
    cv::Mat_<float> layered = m_forest->getLayeredHist(m_img);
    m_forest->getLabelAndConfMat(layered, m_img, confs);

    cv::medianBlur(m_img, m_img, 3);
    processImage();

    cv::imshow("debug PF", m_img);
    m_img = inputImg;
    showDetections();
    imgOut = m_img;
}

void ParticleFilter::processImage()
{
    // new ObjSearch
    initializeParticles();
    for (int i = 0; i < m_num_iters; ++i)
    {
        computeWeights(m_search_particles);
        normalizeWeights(m_search_particles);
        resample(m_search_particles);
    }

//    auto clusters = DBSCAN::getClusters(m_search_particles, m_dbscan_eps, m_dbscan_min_pts);

    // normalize weights
    auto total_weight = sumall(m_search_particles, [](RectangleParticle P) { return P.weight(); });
    for (auto& p : m_search_particles)
        p.setWeight(p.weight() / total_weight);

    // sort descending
    std::sort(std::begin(m_search_particles), std::end(m_search_particles), [](RectangleParticle P1, RectangleParticle P2) {
        return P1.weight() > P2.weight();
    });

    showTopNParticles(m_num_particles_to_display);
}

void ParticleFilter::resample(QVector<RectangleParticle> &particles)
{
    // compute max weight
    auto p_indx = std::max_element(std::begin(particles), std::end(particles), [](RectangleParticle P1, RectangleParticle P2) {
        return P1.weight() < P2.weight();
    });
    auto max_w = particles[std::distance(std::begin(m_search_particles), p_indx)].weight();

    // resample particles
    for (int j = 0; j < m_num_particles; ++j)
    {
        auto p_new = particles[resamplingWheel(m_search_particles, max_w)].clone();
        distortParticle(p_new);             // Motion Update
        m_particlesNew.push_back(p_new);
    }

    // update particles
//    particles.clear();
    particles = m_particlesNew;
    m_particlesNew.clear();
}


void ParticleFilter::showDetections()
{
    auto clusters = DBSCAN::getClusters(m_search_particles, m_dbscan_eps, m_dbscan_min_pts);
    for(auto cluster : clusters)
    {
        int x = 0;
        int y = 0;
        for (auto p : cluster)
        {
            x += p.x();
            y += p.y();
        }
        x = x / cluster.size();
        y = y / cluster.size();
        int x_end = x + m_particle_width;
        int y_end = y + m_particle_height;
        rectangle(m_img, cvPoint(x, y), cv::Point(x_end, y_end), cv::Scalar(0, 130, 0), 2);
    }
}

void ParticleFilter::showTopNParticles(int count)
{
    if (count > m_num_particles)
        count = m_num_particles;
    int x = 0;
    int y = 0;
    for (int i = 0; i < count; ++i)
    {
        x = m_search_particles[i].x();
        y = m_search_particles[i].y();
        int x_end = x + m_particle_width;
        int y_end = y + m_particle_height;
        rectangle(m_img, cv::Point(x, y), cv::Point(x_end, y_end), cv::Scalar(130, 0, 0), 1);
    }
}

void ParticleFilter::initializeParticles()
{
    m_search_particles.clear();
    m_search_particles.resize(m_num_particles);
    for (auto& p : m_search_particles)
    {
        p.setX((int)(m_rand_dice(m_generator)*m_xRange));
        p.setY((int)(m_rand_dice(m_generator)*m_yRange));
        p.setWeight(1.0f / m_num_particles);
        p.setWidth(m_particle_width);
        p.setHeight(m_particle_height);
    }
}

void ParticleFilter::reset()
{
    // TODO: add other reset things
    m_tracking_particles.clear();
}

ParticleFilter::~ParticleFilter()
{
    m_search_particles.clear();
    m_particlesNew.clear();
    delete m_forest;
}
