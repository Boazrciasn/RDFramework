#include "SimplePF.h"
#include <time.h>
#include <opencv2/imgproc.hpp>


SimplePF::SimplePF(int frameWidth, int frameHeight, int nParticles, int nIters, int particleWidth)
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

void SimplePF::run()
{
    //	initializeParticles();
    for (int i = 0; i < m_num_iters; i++)
    {
        resample();
        updateWeights();
        sortParticlesDescending();
        normalize();
    }
    //    showParticles();
    showTopNParticles(m_num_particles_to_display);
}


void SimplePF::initializeParticles()
{
    for (int i = 0; i < m_num_particles; ++i)
    {
        int x = (rand() % (img_width - m_particle_width));
        int y = (rand() % (img_height - m_particle_width));
        float weight = 1.0 / m_num_particles;
        Particle *particle;
        particle = new RectangleParticle(x, y, m_particle_width, weight);
        m_pParticles.push_back(particle);
    }
}

void SimplePF::updateWeights()
{
    for (int i = 0; i < m_num_particles; i++)
        m_pParticles[i]->exec( *&img);
}

void SimplePF::sortParticlesDescending()
{
    for (int i = 0; i < m_num_particles; i++)
        for (int j = i; j < m_num_particles; j++)
            if (m_pParticles[i]->weight() < m_pParticles[j]->weight())
                swap(i, j);
}

void SimplePF::swap(int indA, int indB)
{
    Particle *temp = m_pParticles[indA];
    m_pParticles[indA] = m_pParticles[indB];
    m_pParticles[indB] = temp;
}

void SimplePF::normalize()
{
    float total_weight = 0;
    for (int i = 0; i < m_num_particles; ++i)
        total_weight += m_pParticles[i]->weight();
    for (unsigned int i = 0; i < m_pParticles.size(); ++i)
        m_pParticles[i]->setWeight(m_pParticles[i]->weight() / total_weight);
}

void SimplePF::resample()
{
    for (int j = 0; j < m_num_particles; j++)
    {
        Particle *p_to_distort = m_pParticles[randomParticle()];
        int newX, newY;
        distort(p_to_distort, newX, newY);
        Particle *p_new;
        p_new = new RectangleParticle(newX, newY, m_particle_width, 1.0 / m_num_particles);
        m_pParticlesNew.push_back(p_new);
    }
    updateParticles();
}

int SimplePF::randomParticle()
{
    float rand_number = (float)(rand() % 100) / 100;
    float sum = 0;
    for (int i = 0; i < m_num_particles; i++)
    {
        sum += m_pParticles[i]->weight();
        if (sum >= rand_number)
            return i;
    }
    return 0;
}

void SimplePF::distort(Particle *p, int &x, int &y)
{
    try
    {
        std::normal_distribution<double> distribution(0, 5);
        int dx = (int)distribution(generator);
        int dy = (int)distribution(generator);
        x = p->x();
        y = p->y();
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

void SimplePF::updateParticles()
{
    for (int j = 0; j < m_num_particles; j++)
    {
        m_pParticles[j] = m_pParticlesNew.back();
        m_pParticlesNew.pop_back();
    }
}

void SimplePF::showParticles()
{
    img->copyTo(outIMG);
    int x = 0;
    int y = 0;
    for (int i = 0; i < m_num_particles; ++i)
    {
        x += m_pParticles[i]->x();
        y += m_pParticles[i]->y();
        //int x_end = x + particle_radius;
        //int y_end = y + particle_radius;
        //rectangle(outIMG, cvPoint(x, y), cvPoint(x_end, y_end), cvScalar(130, 0, 0), 1);
    }
    x = x / m_num_particles;
    y = y / m_num_particles;
    int x_end = x + m_particle_width;
    int y_end = y + m_particle_width;
    int r = m_particle_width / 2;;
    if(modelType() == Circle)
        circle(outIMG, cvPoint(x + r, y + r), r, cvScalar(130, 0, 0), 1);
    else if(modelType() == Rectangle)
        rectangle(outIMG, cvPoint(x, y), cvPoint(x_end, y_end), cvScalar(130, 0, 0), 1);
    else
        rectangle(outIMG, cvPoint(x, y), cvPoint(x_end, y_end), cvScalar(130, 0, 0), 1);
}

void SimplePF::showTopNParticles(int count)
{
    img->copyTo(outIMG);
    if(count > m_num_particles)
        count = m_num_particles;
    else if(count == 0)
        showParticles();
    int x = 0;
    int y = 0;
    for (int i = 0; i < count; ++i)
    {
        x = m_pParticles[i]->x();
        y = m_pParticles[i]->y();
        int x_end = x + m_particle_width;
        int y_end = y + m_particle_width;
        rectangle(outIMG, cvPoint(x, y), cvPoint(x_end, y_end), cvScalar(130, 0, 0), 1);
    }
}

SimplePF::~SimplePF() {}
