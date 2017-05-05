#include "precompiled.h"

#include <time.h>

#include "tracking/particlefilter/ParticleFilter.h"
#include "Util.h"
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
    m_distortRange = 10; // TODO: set from gui
    m_rand_dice    = std::uniform_real_distribution<>(0, 1);
    m_rand_distortion = std::uniform_real_distribution<>(-1, 1);

    m_xRange = (img_width - m_particle_width);
    m_yRange = (img_height - m_particle_height);

    // Default
    setDBSCANEps(8.0f);
    setDBSCANMinPts(20);
}

void ParticleFilter::initializeParticles()
{
    m_particles.resize(m_num_particles);
    for (auto& p : m_particles)
    {
        p.setX((int)(m_rand_dice(m_generator)*m_xRange));
        p.setY((int)(m_rand_dice(m_generator)*m_yRange));
        p.setWeight(1.0f / m_num_particles);
        p.setWidth(m_particle_width);
        p.setHeight(m_particle_height);
    }
}

void ParticleFilter::reInitialiaze()
{
    m_particles.clear();
    initializeParticles();
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

    m_img = inputImg;
    showDetections();

    imgOut = m_img;
}

void ParticleFilter::processImage()
{
    auto resamplingWheel = [this]() {
        auto result = std::max_element(std::begin(m_particles), std::end(m_particles), [](RectangleParticle P1, RectangleParticle P2) {
            return P1.weight() < P2.weight();
        });
        auto index = (int)(m_rand_dice(m_generator)*m_num_particles);
        auto mw =  m_particles[std::distance(std::begin(m_particles), result)].weight();
        auto beta = m_rand_dice(m_generator)*2.0f*mw;
        while(beta > m_particles[index].weight())
        {
            beta -= m_particles[index].weight();
            index = (index + 1) % m_num_particles;
        }
        return index;
    };

    reInitialiaze();

    for (int i = 0; i < m_num_iters; ++i)
    {
        for (auto& p : m_particles)
            p.exec(m_img);

        // normalize weights
        auto total_weight = sumall(m_particles, [](RectangleParticle P) { return P.weight(); });
        for (auto& p : m_particles)
            p.setWeight(p.weight() / total_weight);


        // resample particles
        for (int j = 0; j < m_num_particles; ++j)
        {
            auto p_new = m_particles[resamplingWheel()].clone();
            distortParticle(p_new);             // Motion Update
            m_particlesNew.push_back(p_new);
        }

        // update particles
        m_particles = m_particlesNew;
        m_particlesNew.clear();
    }


    // normalize weights
    auto total_weight = sumall(m_particles, [](RectangleParticle P) { return P.weight(); });
    for (auto& p : m_particles)
        p.setWeight(p.weight() / total_weight);

    // sort descending
    std::sort(std::begin(m_particles), std::end(m_particles), [](RectangleParticle P1, RectangleParticle P2) {
        return P1.weight() > P2.weight();
    });

//    showTopNParticles(m_num_particles_to_display);
}

// TODO: double'dan inte cast edilliyor tekrar?
void ParticleFilter::distortParticle(RectangleParticle& p)
{
    if(p.weight() == 0.0)
    {
        p.setX((int)(m_rand_dice(m_generator)*m_xRange));
        p.setY((int)(m_rand_dice(m_generator)*m_yRange));
        p.setWeight(1.0f / m_num_particles);
        p.setWidth(m_particle_width);
        p.setHeight(m_particle_height);
        return;
    }


    int new_x = p.x() + (int)(m_rand_distortion(m_generator)*m_distortRange);
    int new_y = p.y() + (int)(m_rand_distortion(m_generator)*m_distortRange);
    quint16 new_w = p.getWidth() + (int)(m_rand_distortion(m_generator)*m_distortRange);
    quint16 new_h = p.getHeight() + (int)(m_rand_distortion(m_generator)*m_distortRange);
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

void ParticleFilter::showDetections()
{
    auto clusters = DBSCAN::getClusters(m_particles, m_dbscan_eps, m_dbscan_min_pts);

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

void ParticleFilter::showParticles()
{
    int x = 0;
    int y = 0;
    for (int i = 0; i < m_num_particles; ++i)
    {
        x += m_particles[i].x();
        y += m_particles[i].y();
    }
    x = x / m_num_particles;
    y = y / m_num_particles;
    int x_end = x + m_particle_width;
    int y_end = y + m_particle_height;
    rectangle(m_img, cvPoint(x, y), cv::Point(x_end, y_end), cv::Scalar(130, 0, 0), 1);
}

void ParticleFilter::showTopNParticles(int count)
{
    if (count > m_num_particles)
        count = m_num_particles;
    else if (count == 0)
        showParticles();
    int x = 0;
    int y = 0;
    for (int i = 0; i < count; ++i)
    {
        x = m_particles[i].x();
        y = m_particles[i].y();
        int x_end = x + m_particle_width;
        int y_end = y + m_particle_height;
        rectangle(m_img, cvPoint(x, y), cv::Point(x_end, y_end), cv::Scalar(130, 0, 0), 1);
    }
}

ParticleFilter::~ParticleFilter()
{
    m_particles.clear();
    m_particlesNew.clear();
    delete m_forest;
}
