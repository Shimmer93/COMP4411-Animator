#pragma once

#include "vec.h"
#include "particle.h"

class Force
{
public:
	virtual Vec3f apply(const Particle* par, float t) = 0;
};

class Wind : public Force
{
public:
	Wind(Vec3f f): f(f) {}
	virtual Vec3f apply(const Particle* par, float t);
private:
	Vec3f f;
};

class Gravity : public Force
{
public:
	Gravity(float g) : g(g) {}
	virtual Vec3f apply(const Particle* par, float t);
private:
	float g;
};

class Viscous : public Force
{
public:
	Viscous(float k): k(k) {}
	virtual Vec3f apply(const Particle* par, float t);
private:
	float k;
};

class TestForce : public Force
{
public:
	virtual Vec3f apply(const Particle* par, float t);
};

class Spring
{
public:
	Spring(float ks, float kd, float r, Particle* par1, Particle* par2) 
		: ks(ks), kd(kd), r(r), par1(par1), par2(par2) {}
	void apply();

private:
	float ks, kd;
	float r;
	Particle* par1;
	Particle* par2;
};