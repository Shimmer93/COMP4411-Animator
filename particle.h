 // SAMPLE_SOLUTION
#pragma once

#include "vec.h"
#include <vector>
using namespace std;

class Particle
{
public:
	Particle() : m(0.0), p(Vec3f(0.0, 0.0, 0.0)), v(Vec3f(0.0, 0.0, 0.0)), f(Vec3f(0.0, 0.0, 0.0)) {}
	Particle(float mass, Vec3f position, Vec3f velocity, Vec3f force)
		: m(mass), p(position), v(velocity), f(force) {}

	void draw(const Vec3f& camPos);

	float m;
	Vec3f p;
	Vec3f v;
	Vec3f f;
};