#include "force.h"
#include <iostream>
using namespace std;

Vec3f Wind::apply(const Particle* par, float t)
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

Vec3f TestForce::apply(const Particle* par, float t)
{
	if (t < 0.1 && par->p[0]>1 && par->p[1]>1) {
		return Vec3f(0.0, 0.0, 0.5);
	}
	else {
		return Vec3f(0, 0, 0);
	}
	/*if (par->p[1] < 1e-5) {
		return Vec3f(0.0, par->m * 5.0, 0.0);
	}
	else {
		return Vec3f(0.0, 0.0, 0.0);
	}*/
}

void Spring::apply()
{
	Vec3f x = par1->p - par2->p;
	Vec3f v = par1->v - par2->v;
	Vec3f fs = -(ks * (x.length() - r) + kd * (v * x) / x.length()) * x / x.length();
	par1->f += fs;
	par2->f += -fs;
}