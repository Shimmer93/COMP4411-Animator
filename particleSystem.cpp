#pragma warning(disable : 4786)

#include "particleSystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

float frand() {
	return rand() / (float)RAND_MAX;
}


/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem(vector<Force*> forces, float fps)
	: particles(), baked_data(), forces(forces),
	  bake_fps(fps), bake_start_time(0.0), bake_end_time(0.0), simulate(false), dirty(false)
{}


/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
	// TODO

}


/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
    
	bake_start_time = t;

	// These values are used by the UI ...
	// -ve bake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
	bake_end_time = -1;
	simulate = true;
	dirty = true;

}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
    
	// TODO
	bake_end_time = t;

	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
    
	// TODO
	bake_start_time = 0;
	bake_end_time = 0;

	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{

	float prev_t = t - 1.0 / bake_fps;
	for (auto par : particles) {
		par->p += (1.0 / bake_fps) * par->v;
		par->v += (1.0 / bake_fps) * applyForces(par, prev_t) / par->m;
	}

	if (simulate) {
		bakeParticles(t);
	}

}

/** Render particles */
void ParticleSystem::drawParticles(float t)
{
	if (baked_data.count(t) > 0) {
		for (auto par : baked_data.at(t))
			par->draw();
	}
	else if (simulate){
		computeForcesAndUpdateParticles(t);
		for (auto par : particles)
			par->draw();
	}
}

/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t) 
{
	if(t >= bake_start_time && (t <= bake_end_time || bake_end_time == -1))
		baked_data[t].assign(particles.begin(), particles.end());
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
	baked_data.clear();
}

void ParticleSystem::spawnParticles(Mat4f cameraMatrix, Mat4f modelViewMatrix, int numParticles)
{
	Mat4f worldMatrix = cameraMatrix.inverse() * modelViewMatrix;
	Vec4f worldPoint = worldMatrix * Vec4f(0.0, 0.0, 0.0, 1.0);

	for (int i = 0; i < numParticles; i++) {
		float randX = (frand() - 0.5) * 0.5;
		float randY = (frand() - 0.5) * 0.5;
		float randZ = (frand() - 0.5) * 0.5;
		float randVX = (frand() - 0.5) * 1;
		float randVY = (frand() - 0.5) * 1;
		float randVZ = (frand() - 0.5) * 1;
		Particle* par = new Particle(0.1, Vec3f(worldPoint[0] + randX, worldPoint[1] + randY, worldPoint[2] + randZ), Vec3f(randVX, randVY, 5+randVZ));
		particles.push_back(par);
	}
}

void ParticleSystem::clearParticles()
{
	particles.clear();
}

Vec3f ParticleSystem::applyForces(const Particle* par, float t)
{
	Vec3f result(0.0, 0.0, 0.0);
	for (auto f : forces)
		result += f->apply(par, t);
	return result;
}