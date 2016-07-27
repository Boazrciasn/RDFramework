#pragma once
#include <iostream>
#include <random>

#include "Particle.h"
#include "RectangleParticle.h"
#include <tbb/tbb.h>

class SimplePF{
public:
    SimplePF(int frameWidth, int frameHeight, int nParticles, int nIters, int particleWidth);

	cv::Mat getIMG() { return outIMG; }

    void setIMG(cv::Mat *pImage) {
        img = pImage;
    }

    inline int numParticles() const { return m_num_particles; }
	inline void setNumParticles(int value) { m_num_particles = value; }

    inline int numIters() const { return m_num_iters; }
    inline void setNumIters(int value) { m_num_iters = value; }

    inline int particleWidth() const { return m_particle_width; }
    inline void setParticleWidth(int value) { m_particle_width = value; }

    inline int modelType() const { return type; }
    inline void setModelType(int value) { type = value; }

    inline int getRationOfTop(int count){
        float total_weight = 0;
        for (int i = 0; i < count; ++i)
            total_weight += m_pParticles[i]->weight();
        return total_weight*100;
    }

    inline void setParticlesToDisplay(int value) { m_num_particles_to_display = value; }

	void run();

    // display particles
    void showParticles();
    void showTopNParticles(int count);

	~SimplePF();

private:
    // creates random particles with equal weights
    void initializeParticles();

	// traverse all particles and checks the particle's value against our model's values
	// and update the weights
    void updateWeights();

	// sort according to weights
	void sortParticlesDescending();
	void swap(int indA, int indB);

	// normalize particle weights
	void normalize();

	// creates new particles around the highly weighted particles
	void resample();
	int randomParticle();

	// distort x or y by a standart deviation
	void distort(Particle *p, int &x, int &y);
	void updateParticles();

	// variables 
	int m_num_particles;
	int m_num_iters;
    int m_particle_width;
    int m_num_particles_to_display;

    int type = Rectangle;

	std::vector<Particle *> m_pParticles;
    std::vector<Particle *> m_pParticlesNew;

    cv::Mat *img;
	cv::Mat outIMG;
	int img_height, img_width;
	std::default_random_engine generator;
	
};

