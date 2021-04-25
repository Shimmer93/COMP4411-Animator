#pragma once

#include "vec.h"
#include "particle.h"

class Force
{
public:
	virtual Vec3f apply(const Particle* par, float t) = 0;
};

class ConstForce : public Force
{
public:
	ConstForce(Vec3f f): f(f) {}
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

