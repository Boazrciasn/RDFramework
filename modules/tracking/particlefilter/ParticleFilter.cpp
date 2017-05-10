#include "precompiled.h"

#include <time.h>
#include <chrono>

#include "tracking/particlefilter/ParticleFilter.h"
#include "Target.h"
#include "tracking/particlefilter/RectangleParticle.h"

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
    m_distortRange = 10; // TODO: set from gui
    m_nTrackers = 30;   // TODO: set from gui
    m_rand_dice    = std::uniform_real_distribution<>(0, 1);
    m_rand_distortion = std::normal_distribution<>();

    m_xRange = (img_width - m_particle_width);
    m_yRange = (img_height - m_particle_height);

    // Default
    setDBSCANEps(8.0f);
    setDBSCANMinPts(20);
}

void ParticleFilter::exec(const cv::Mat &inputImg, cv::Mat &imgOut)
{
    m_img = inputImg.clone();
    m_tmp_debug = inputImg.clone();
    cv::cvtColor(m_img,m_img, CV_BGR2Lab);
    cv::Mat_<float> confs{};
    cv::Mat_<float> layered = m_forest->getLayeredHist(m_img);
    m_forest->getLabelAndConfMat(layered, m_img, confs);

    cv::medianBlur(m_img, m_img, 3);
    processImage();

    cv::imshow("debug PF", m_img);
    m_img = inputImg;
//    showTopNParticles(m_tracking_particles.size());
    showRects(m_img, m_tracked_clusters, cv::Scalar(0, 130, 0), 2);
    imgOut = m_img;
}



void ParticleFilter::processImage()
{
    for (int i = 0; i < 2; ++i)
    {
        computeWeights(m_tracking_particles);
        normalizeWeights(m_tracking_particles);
        resample(m_tracking_particles, 1);
    }

    // new ObjSearch
    initializeParticles();
    for (int i = 0; i < m_num_iters; ++i)
    {
        computeWeights(m_search_particles);
        normalizeWeights(m_search_particles);
        resample(m_search_particles, 0);
    }

    m_tracked_clusters = DBSCAN::getClusters_(m_tracking_particles, m_dbscan_eps, m_dbscan_min_pts);
    addNewTrackers();
}

void ParticleFilter::resample(QVector<RectangleParticle> &particles, quint8 updateOrNot)
{
    auto nParticles = particles.size();
    if(nParticles == 0) return;
    // compute max weight
    auto p_indx = std::max_element(std::begin(particles), std::end(particles), [](RectangleParticle P1, RectangleParticle P2) {
        return P1.weight < P2.weight;
    });
    auto max_w = particles[std::distance(std::begin(particles), p_indx)].weight;

    // resample particles
    for (int j = 0; j < nParticles; ++j)
    {
        auto p_new = particles[resamplingWheel(particles, max_w)].clone();
        if(p_new.weight < 1.0f/(1*nParticles))
            continue;
        distortParticle(p_new, updateOrNot);             // Motion Update
        m_particlesNew.push_back(p_new);
    }

    // update particles
    particles.clear();
    particles = m_particlesNew;
    m_particlesNew.clear();
}


void ParticleFilter::showDetections()
{
    for(auto cluster : m_tracked_clusters)
        cv::rectangle(m_img, cluster, cv::Scalar(0, 130, 0), 2);
}

void ParticleFilter::showTopNParticles(int count)
{
    normalizeWeights(m_tracking_particles);
    auto size = m_tracking_particles.size();
    // sort descending
    std::sort(std::begin(m_tracking_particles), std::end(m_tracking_particles), [](RectangleParticle P1, RectangleParticle P2) {
        return P1.weight > P2.weight;
    });

    if (count > size)
        count = size;
    int x = 0;
    int y = 0;
    for (auto i = 0; i < count; ++i)
    {
        auto& p = m_tracking_particles[i];
        x = p.x;
        y = p.y;
        int x_end = x + p.width;
        int y_end = y + p.height;
        rectangle(m_img, cv::Point(x, y), cv::Point(x_end, y_end), cv::Scalar(130, 0, 0), 1);
    }
}

void ParticleFilter::initializeParticles()
{
    m_search_particles.clear();
    m_search_particles.resize(m_num_particles);
    for (auto& p : m_search_particles)
    {
        p.x = (int)(m_rand_dice(m_generator)*m_xRange);
        p.y = (int)(m_rand_dice(m_generator)*m_yRange);
        p.width = m_particle_width;
        p.height = m_particle_height;
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
