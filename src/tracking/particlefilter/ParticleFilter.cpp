#include "particlefilter/ParticleFilter.h"
#include <time.h>
#include <opencv2/imgproc.hpp>


ParticleFilter::ParticleFilter(int frameWidth, int frameHeight, int nParticles, int nIters,
                               int particleWidth)
{
    img_height = frameHeight;
    img_width = frameWidth;
    m_num_particles_to_display = 0;
    setNumParticles(nParticles);
    setNumIters(nIters);
    setParticleWidth(particleWidth);
    setParticlesToDisplay(m_num_particles_to_display);
    srand(time(NULL));
    initializeParticles();
}

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
        float weight = 1.0 / m_num_particles;
        Particle *particle;
        particle = new RectangleParticle(x, y, m_particle_width, weight, m_Targets.at(0)->getHist(), m_histSize);
        m_Particles.push_back(particle);
    }
}

void ParticleFilter::updateWeights()
{
    for (int i = 0; i < m_num_particles; i++)
        m_Particles[i]->exec( *&m_img);
}

void ParticleFilter::sortParticlesDescending()
{
    for (int i = 0; i < m_num_particles; i++)
        for (int j = i; j < m_num_particles; j++)
            if (m_Particles[i]->getWeight() < m_Particles[j]->getWeight())
                swap(i, j);
}

void ParticleFilter::swap(int indA, int indB)
{
    Particle *temp = m_Particles[indA];
    m_Particles[indA] = m_Particles[indB];
    m_Particles[indB] = temp;
}

void ParticleFilter::normalizeWeights()
{
    float total_weight = 0;
    for (int i = 0; i < m_num_particles; ++i)
        total_weight += m_Particles[i]->getWeight();
    for (unsigned int i = 0; i < m_Particles.size(); ++i)
        m_Particles[i]->setWeight(m_Particles[i]->getWeight() / total_weight);
}

void ParticleFilter::resampleParticles()
{
    for (int j = 0; j < m_num_particles; j++)
    {
        Particle *p_to_distort = m_Particles[randomParticle()];
        int newX, newY;
        distortParticle(p_to_distort, newX, newY);
        Particle *p_new;
        p_new = new RectangleParticle(newX, newY, m_particle_width, 1.0 / m_num_particles, m_Targets.at(0)->getHist(),
                                      m_histSize);
        m_ParticlesNew.push_back(p_new);
    }
    updateParticles();
}

int ParticleFilter::randomParticle()
{
    float rand_number = (float)(rand() % 100) / 100;
    float sum = 0;
    for (int i = 0; i < m_num_particles; i++)
    {
        sum += m_Particles[i]->getWeight();
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
    for (int j = 0; j < m_num_particles; j++)
    {
        m_Particles[j] = m_ParticlesNew.back();
        m_ParticlesNew.pop_back();
    }
}

void ParticleFilter::showParticles()
{
    m_img->copyTo(m_outIMG);
    int x = 0;
    int y = 0;
    for (int i = 0; i < m_num_particles; ++i)
    {
        x += m_Particles[i]->getX();
        y += m_Particles[i]->getY();
    }
    x = x / m_num_particles;
    y = y / m_num_particles;
    int x_end = x + m_particle_width;
    int y_end = y + m_particle_width;
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
        x = m_Particles[i]->getX();
        y = m_Particles[i]->getY();
        int x_end = x + m_particle_width;
        int y_end = y + m_particle_width;
        rectangle(m_outIMG, cvPoint(x, y), cvPoint(x_end, y_end), cvScalar(130, 0, 0), 1);
    }
}

ParticleFilter::~ParticleFilter() {}
