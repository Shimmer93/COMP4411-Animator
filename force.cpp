#include "force.h"
#include <iostream>
using namespace std;

Vec3f ConstForce::apply(const Particle* par, float t)
{
	return f;
}

Vec3f Gravity::apply(const Particle* par, float t)
{
	return Vec3f(0.0, -par->m * g, 0.0);
}

Vec3f Viscous::apply(const Particle* par, float t)
{
	return -k * par->v;
}