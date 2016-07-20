#pragma once
#include <iostream>
#include <random>

#include "Particle.h"
#include "RectangleParticle.h"

class SimplePF{
public:
    SimplePF(cv::Mat *pImage, int nParticles, int nIters);

	cv::Mat getIMG() { return outIMG; }

    void setIMG(cv::Mat *pImage) {
        img = pImage;
        img_height = img->rows;
        img_width = img->cols;
	}

    inline int numParticles() const { return m_num_particles; }
	inline void setNumParticles(int value) { m_num_particles = value; }

    inline int numIters() const { return m_num_iters; }
    inline void setNumIters(int value) { m_num_iters = value; }

    inline int particleWidth() const { return m_particle_width; }
    inline void setParticleWidth(int value) { m_particle_width = value; }

    inline int modelType() const { return type; }
    inline void setModelType(int value) { type = value; }

	void run();

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


    // display particles
    void showParticles();
    void showTopNParticles(int count);

	// variables 
	int m_num_particles;
	int m_num_iters;
    int m_particle_width;

    int type = Rectangle;

	std::vector<Particle *> m_pParticles;
    std::vector<Particle *> m_pParticlesNew;

    cv::Mat *img;
	cv::Mat outIMG;
	int img_height, img_width;
	std::default_random_engine generator;
	
};

