#pragma warning(disable : 4786)

#include "particleSystem.h"
#include "modelerapp.h"
#include "modelerui.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <algorithm>
#include <iostream>
using namespace std;

float frand() {
	return rand() / (float)RAND_MAX;
}


/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem(vector<Force*> forces, bool collide)
	: camera(nullptr), particles(), baked_data(), forces(forces),
	  bake_fps(-1), bake_start_time(0.0), bake_end_time(0.0), simulate(false), dirty(false), collide(collide)
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
    
	if (!simulate) {
		clearBaked();
		bake_start_time = t;
	}

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
	if (simulate)
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
	if (bake_fps <= 0) return;

	if (collide)
		detectCollision(0.2+1e-6);

	for (Particle& par : particles)
		updateParticle(par, t, true);

	if (simulate)
		bakeParticles(t);

}

/** Render particles */
void ParticleSystem::drawParticles(float t)
{
	if (camera == nullptr)
		return;

	if (baked_data.count(t) > 0) {
		for (Particle& par : baked_data.at(t))
			par.draw(camera->getPosition());
	}
	else if (simulate){
		computeForcesAndUpdateParticles(t);
		for (Particle& par : particles)
			par.draw(camera->getPosition());
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
		// add some randomness for better effects
		float randX = (frand() - 0.5) * 0.5;
		float randY = (frand() - 0.5) * 0.5;
		float randZ = (frand() - 0.5) * 0.5;
		float randVX = (frand() - 0.5) * 1;
		float randVY = (frand() - 0.5) * 1;
		float randVZ = (frand() - 0.5) * 1;
		Particle par(0.1, Vec3f(worldPoint[0] + randX, worldPoint[1] + randY, worldPoint[2] + randZ + 1.8), Vec3f(randVX, randVY, 5+randVZ), Vec3f(0, 0, 0));
		particles.push_back(par);
	}
}

void ParticleSystem::clearParticles()
{
	particles.clear();
}

void ParticleSystem::updateParticle(Particle& par, float t, bool clear)
{
	// Runge-Kutta method
	float dt = 1.0 / bake_fps;
	float prev_t = t - dt;

	applyForces(par, prev_t, clear);
	Vec3f dp1 = dt * par.v;
	Vec3f dv1 = dt * par.f / par.m;
	Particle temp_par(par);
	temp_par.p += dp1;
	temp_par.v += dv1;
	applyForces(temp_par, t, true);
	Vec3f dp2 = dt * temp_par.v;
	Vec3f dv2 = dt * temp_par.f / temp_par.m;
	par.p += (dp1 + dp2) * 0.5;
	par.v += (dv1 + dv2) * 0.5;
}

void ParticleSystem::applyForces(Particle& par, float t, bool clear)
{
	if(clear)
		par.f = Vec3f(0.0, 0.0, 0.0);
	if (forces.empty())
		return;
	for (auto force : forces)
		par.f += force->apply(&par, t);
}

void ParticleSystem::sortParticles()
{
	sort(particles.begin(), particles.end(), [this](const Particle& par1, const Particle& par2) {
		return this->compareParticles(par1, par2);});
}

bool ParticleSystem::compareParticles(const Particle& par1, const Particle& par2)
{
	float dis1 = (camera->getPosition() - par1.p).length2();
	float dis2 = (camera->getPosition() - par2.p).length2();

	return dis1 > dis2;
}

void ParticleSystem::detectCollision(float thresh)
{
	int numPar = particles.size();
	if (numPar < 2) return;

	for (int i = 0; i < numPar - 1; i++) {
		for (int j = i + 1; j < numPar; j++) {
			Particle& par1 = particles[i];
			Particle& par2 = particles[j];
			Vec3f d = par1.p - par2.p;
			if (d.length() < thresh && par1.v * d < 0 && par2.v * d > 0) {
				// perfect elastic collision
				float m1 = par1.m;
				float m2 = par2.m;

				Vec3f vd1 = par1.v * d / d.length2() * d;
				Vec3f vd2 = par2.v * d / d.length2() * d;
				Vec3f vdc1 = ((m1 - m2) * vd1 + 2 * m2 * vd2) / (m1 + m2);
				Vec3f vdc2 = ((m2 - m1) * vd2 + 2 * m1 * vd1) / (m1 + m2);

				par1.v += vdc1 - vd1;
				par2.v += vdc2 - vd2;
			}
		}
	}
}

void ParticleSystem::addParticle(const Particle& par)
{
	particles.push_back(par);
}