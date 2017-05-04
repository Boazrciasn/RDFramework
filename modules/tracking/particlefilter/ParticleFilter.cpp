#include "precompiled.h"

#include <time.h>

#include "tracking/particlefilter/ParticleFilter.h"
#include "Util.h"
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
    m_rand_dice    = std::uniform_real_distribution<>(0, 1);
    m_rand_distortion = std::uniform_real_distribution<>(-1, 1);
    m_bsMoG = cv::cuda::createBackgroundSubtractorGMG(5);

    m_xRange = (img_width - m_particle_width);
    m_yRange = (img_height - m_particle_height);
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

    // Background Subtraction
//    cv::cuda::GpuMat src(inputImg);
//    cv::cuda::GpuMat des;
//    m_bsMoG->apply(src, des);
//    des.download(imgOut);


    cv::cvtColor(m_img,m_img, CV_BGR2Lab);
    cv::Mat_<float> confs{};
    cv::Mat_<float> layered = m_forest->getLayeredHist(m_img);
    m_forest->getLabelAndConfMat(layered, m_img, confs);

    cv::medianBlur(m_img, m_img, 3);
    m_img(cv::Range(0,240), cv::Range::all()) = 0;

//    m_img.copyTo(imgOut, imgOut);
//    m_img = imgOut;
    processImage();

//    m_bsMoG->apply(inputImg, imgOut);
//    m_img.copyTo(imgOut, imgOut);
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
        // resample particles
        auto ratio = 0.9f;
        auto resample_count = (int)(ratio*m_num_particles);
        for (int j = 0; j < resample_count; ++j)
        {
            auto p_new = m_particles[resamplingWheel()].clone();
            distortParticle(p_new);             // Motion Update
            p_new.exec(m_img);                  // update weight
            m_particlesNew.push_back(p_new);
        }

        for (int j = 0; j < m_num_particles-resample_count; ++j)
        {
            RectangleParticle p_new;
            p_new.setX((int)(m_rand_dice(m_generator)*m_xRange));
            p_new.setY((int)(m_rand_dice(m_generator)*m_yRange));
            p_new.setWeight(1.0f / m_num_particles);
            p_new.setWidth(m_particle_width);
            p_new.setHeight(m_particle_height);
            m_particlesNew.push_back(p_new);
        }

        // update particles
        m_particles = m_particlesNew;
        m_particlesNew.clear();

        // normalize weights
        auto total_weight = sumall(m_particles, [](RectangleParticle P) { return P.weight(); });
        for (auto& p : m_particles)
            p.setWeight(p.weight() / total_weight);
    }

    // sort descending
    std::sort(std::begin(m_particles), std::end(m_particles), [](RectangleParticle P1, RectangleParticle P2) {
        return P1.weight() > P2.weight();
    });

    showTopNParticles(m_num_particles_to_display);
}

// TODO: double'dan inte cast edilliyor tekrar?
void ParticleFilter::distortParticle(RectangleParticle& p)
{
    int newx = p.x() + (int)(m_rand_distortion(m_generator)*m_distortRange);
    int newy = p.y() + (int)(m_rand_distortion(m_generator)*m_distortRange);
    if (newx < m_xRange && newx > 0)
        p.setX(newx);
    if (newy < m_yRange && newy > 0)
        p.setY(newy);
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
    rectangle(m_img, cvPoint(x, y), cvPoint(x_end, y_end), cvScalar(130, 0, 0), 1);
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
        rectangle(m_img, cvPoint(x, y), cvPoint(x_end, y_end), cvScalar(130, 0, 0), 1);
    }
}

ParticleFilter::~ParticleFilter()
{
    m_particles.clear();
    m_particlesNew.clear();
    delete m_forest;
}
