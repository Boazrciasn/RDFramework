#include "precompiled.h"
#include <chrono>
#include "tracking/particlefilter/ParticleFilter.h"

ParticleFilter::ParticleFilter(
        uint16_t frameWidth, uint16_t frameHeight, uint16_t nParticles, uint8_t nIters,
        uint16_t particleWidth, uint16_t particleHeight) : m_img_width(frameWidth), m_img_height(frameHeight),
                                                           m_num_particles(nParticles), m_num_iters(nIters),
                                                           m_particle_width(particleWidth),
                                                           m_particle_height(particleHeight) {
    m_rand_dice = std::uniform_real_distribution<>(0, 1);
    m_rand_distortion = std::normal_distribution<>();
    m_xRange = (m_img_width - m_particle_width);
    m_yRange = (m_img_height - m_particle_height);

    // Default
    setDBSCANEps(8.0f);
    setDBSCANMinPts(20);
}

void ParticleFilter::exec(const cv::Mat &inputImg, cv::Mat &imgOut)
{
    cv::cvtColor(inputImg, m_img, CV_BGR2Lab);
    m_img = m_forest->computeLabels(m_img);
    cv::medianBlur(m_img, m_img, 3);
//    SimpleBlobDetector detector;

    cv::integral(m_img, m_integralMat);
    processImage();
    cv::imshow("debug PF", m_img);
    m_img = inputImg;


    m_tracked_clusters = DBSCAN::getClusters_(m_tracking_particles, m_dbscan_eps, m_dbscan_min_pts);
//    qDebug() << "Cluster count: " << m_tracked_clusters.size();
    addNewTrackers();
//    showTopNParticles(m_tracking_particles.size());
//    showRects(m_img, m_tracked_clusters, cv::Scalar(0, 130, 0), 2);
    showDetections(m_search_particles, m_img);
    imgOut = m_img;
//    addNewTrackers();
}



void ParticleFilter::processImage()
{
//    for (int i = 0; i < 1; ++i)
    {
        computeWeights(m_tracking_particles);
        normalizeWeights(m_tracking_particles);
        resample(m_tracking_particles, false);
    }
    computeWeights(m_tracking_particles);
    normalizeWeights(m_tracking_particles);

    // new ObjSearch
    initializeParticles();
    for (int i = 0; i < m_num_iters; ++i)
    {
        computeWeights(m_search_particles);
        normalizeWeights(m_search_particles);
        resample(m_search_particles, true);
    }
    computeWeights(m_search_particles);
    normalizeWeights(m_search_particles);
}

void ParticleFilter::resample(QVector<RectangleParticle> &particles, bool isSearch) {
    auto nParticles = particles.size();
    if (!isSearch)
        nParticles = m_nTrackers * m_tracked_clusters.size();
    if (nParticles == 0) return;
    // compute max weight
    auto p_indx = std::max_element(std::begin(particles), std::end(particles),
                                   [](RectangleParticle P1, RectangleParticle P2) {
                                       return P1.weight < P2.weight;
                                   });
    auto max_w = particles[std::distance(std::begin(particles), p_indx)].weight;

    // resample particles
    for (int j = 0; j < nParticles; ++j) {
        auto p_new = particles[resamplingWheel(particles, max_w)].clone();
        if (p_new.weight < 1.0f / (1 * nParticles))
            continue;
        isSearch ? distortSearchParticle(p_new) : distortParticle(p_new);
        m_particlesNew.push_back(p_new);
    }

    for (auto &p : m_particlesNew)
        ++p.age;

    if (!isSearch)
        qDebug() << nParticles;

    // update particles
    particles.clear();
    particles = m_particlesNew;
    m_particlesNew.clear();
}


void ParticleFilter::showTopNParticles(int count) {
    auto size = m_tracking_particles.size();
    // sort descending
    std::sort(std::begin(m_tracking_particles), std::end(m_tracking_particles),
              [](RectangleParticle P1, RectangleParticle P2) {
                  return P1.weight > P2.weight;
              });

    count = (count > size) ? size : count;
    for (auto i = 0; i < count; ++i) {
        auto &p = m_tracking_particles[i];
        auto rgb = colors.colors[p.cluster % colors.colors.size()];
        rectangle(m_img, p.boundingBox(), cv::Scalar(rgb[0], rgb[1], rgb[2]), 1);
    }
}

void ParticleFilter::initializeParticles() {
    m_search_particles.clear();
    m_search_particles.resize(m_num_particles);
    for (auto &p : m_search_particles) {
        p.x = uint16_t(m_rand_dice(m_generator) * m_xRange);
        p.y = uint16_t(m_rand_dice(m_generator) * m_yRange);
        p.width = m_particle_width;
        p.height = m_particle_height;
    }
}

void ParticleFilter::reset() {
    // TODO: add other reset things
    m_tracking_particles.clear();
}

ParticleFilter::~ParticleFilter() {
    m_search_particles.clear();
    m_particlesNew.clear();
    delete m_forest;
}
