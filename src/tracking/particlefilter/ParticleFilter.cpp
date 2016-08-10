#include "src/util/precompiled.h"

#include <time.h>

#include "particlefilter/ParticleFilter.h"
#include "Util.h"
#include "Target.h"
#include "particlefilter/RectangleParticle.h"

ParticleFilter::ParticleFilter(int frameWidth, int frameHeight, int nParticles, int nIters,
                               int particleWidth, int particleHeight, Target *target)
{
    type = Rectangle;

    m_target = target;
    img_height = frameHeight;
    img_width = frameWidth;
    m_num_particles_to_display = 0;
    setNumParticles(nParticles);
    setNumIters(nIters);
    setParticleWidth(particleWidth);
    setParticleHeight(particleHeight);
    setParticlesToDisplay(m_num_particles_to_display);
    srand(time(nullptr));
    initializeParticles();
}

int ParticleFilter::getRatioOfTop(int count)
{
    float total_weight = 0;
    for (int i = 0; i < count; ++i)
        total_weight += m_particles[i]->getWeight();
    return total_weight * 100;
}

void ParticleFilter::setVideoReader(VideoReader *videoReader) {m_VideoReader = videoReader; }

void ParticleFilter::processImage()
{
    for (int i = 0; i < m_num_iters; i++)
    {
        resampleParticles();
        updateWeights();
        sortParticlesDescending();
        normalizeWeights();
    }
    showTopNParticles(m_num_particles_to_display);
}


void ParticleFilter::initializeParticles()
{
    for (int i = 0; i < m_num_particles; ++i)
    {
        int x = (rand() % (img_width - m_particle_width));
        int y = (rand() % (img_height - m_particle_width));
        auto weight = 1.0f / m_num_particles;
        Particle *particle = new RectangleParticle(x, y, m_particle_width, m_particle_height, weight, m_target->getHist(), m_histSize);
        m_particles.push_back(particle);
    }
}

void ParticleFilter::updateWeights()
{
    for (int i = 0; i < m_num_particles; ++i)
        m_particles[i]->exec(*&m_img);
}

void ParticleFilter::sortParticlesDescending()
{
    using namespace std;
    sort(begin(m_particles), end(m_particles), [](Particle *P1, Particle *P2) {
        return P1->getWeight() > P2->getWeight();
    });
}

void ParticleFilter::normalizeWeights()
{
    auto total_weight = sumall(m_particles, [](Particle *P) { return P->getWeight(); });

    auto nParticles = m_particles.size();

//    double total_weight = 0;
//    for (int i = 0; i < nParticles; ++i)
//        total_weight += m_particles[i]->getWeight();

    for (unsigned int i = 0; i < nParticles; ++i)
        m_particles[i]->setWeight(m_particles[i]->getWeight() / total_weight);
}

void ParticleFilter::resampleParticles()
{
    for (int j = 0; j < m_num_particles; ++j)
    {
        Particle *p_to_distort = m_particles[randomParticle()];
        int newX, newY;
        distortParticle(p_to_distort, newX, newY);
        m_newCoordinates.push_back(cv::Point(newX,newY));
    }
    updateParticles();
}

int ParticleFilter::randomParticle()
{
    float rand_number = (float)(rand() % 100) / 100;
    float sum = 0;
    for (int i = 0; i < m_num_particles; i++)
    {
        sum += m_particles[i]->getWeight();
        if (sum >= rand_number)
            return i;
    }
    return 0;
}

void ParticleFilter::distortParticle(Particle *p, int &x, int &y)
{
    try
    {
        std::normal_distribution<double> distribution(0, 5);
        int dx = (int)distribution(generator);
        int dy = (int)distribution(generator);
        x = p->getX();
        y = p->getY();
        int newx = x + dx;
        int newy = y + dy;
        if (newx < img_width - m_particle_width && newx > 0)
            x = newx;
        if (newy < img_height - m_particle_width && newy > 0)
            y = newy;
    }
    catch (cv::Exception &e)
    {
        std::cout << "Distribution Error" << e.msg;
    }
}

void ParticleFilter::updateParticles()
{
    for (int i = 0; i < m_num_particles; ++i)
    {
        Particle *p_particle = m_particles[i];
        p_particle->setCoordinates(m_newCoordinates[i]);
        p_particle->setWeight(1.0 / m_num_particles);
    }
    m_newCoordinates.clear();
}

void ParticleFilter::showParticles()
{
    m_img->copyTo(m_outIMG);
    int x = 0;
    int y = 0;
    for (int i = 0; i < m_num_particles; ++i)
    {
        x += m_particles[i]->getX();
        y += m_particles[i]->getY();
    }
    x = x / m_num_particles;
    y = y / m_num_particles;
    int x_end = x + m_particle_width;
    int y_end = y + m_particle_height;
    int r = m_particle_width / 2;;
    if (getModelType() == Circle)
        circle(m_outIMG, cvPoint(x + r, y + r), r, cvScalar(130, 0, 0), 1);
    else if (getModelType() == Rectangle)
        rectangle(m_outIMG, cvPoint(x, y), cvPoint(x_end, y_end), cvScalar(130, 0, 0), 1);
    else
        rectangle(m_outIMG, cvPoint(x, y), cvPoint(x_end, y_end), cvScalar(130, 0, 0), 1);
}

void ParticleFilter::showTopNParticles(int count)
{
    m_img->copyTo(m_outIMG);
    if (count > m_num_particles)
        count = m_num_particles;
    else if (count == 0)
        showParticles();
    int x = 0;
    int y = 0;
    for (int i = 0; i < count; ++i)
    {
        x = m_particles[i]->getX();
        y = m_particles[i]->getY();
        int x_end = x + m_particle_width;
        int y_end = y + m_particle_height;
        rectangle(m_outIMG, cvPoint(x, y), cvPoint(x_end, y_end), cvScalar(130, 0, 0), 1);
    }
}

ParticleFilter::~ParticleFilter() {}
