 // SAMPLE_SOLUTION
#pragma once

#include "vec.h"

class Particle
{
public:
	Particle() : m(0.0), p(Vec3f(0.0, 0.0, 0.0)), v(Vec3f(0.0, 0.0, 0.0)) {}
	Particle(float mass, Vec3f position, Vec3f velocity)
		: m(mass), p(position), v(velocity) {}

	void draw(const Vec3f& camPos);

	float m;
	Vec3f p;
	Vec3f v;

};