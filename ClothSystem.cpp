#include "ClothSystem.h"
#include "modelerdraw.h"
#include <iostream>
using namespace std;

#define SQRT2 1.414213562373095

ClothSystem::ClothSystem(vector<Force*> forces, float gridSize, int height, int width, 
	  Vec3f startPoint, float ks, float kd)
	: ParticleSystem(forces, false), gridSize(gridSize), height(height), width(width)
{
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			particles.push_back(new Particle(0.1, startPoint + Vec3f(i * gridSize, 0, j * gridSize),
				Vec3f(0, 0, 0), Vec3f(0, 0, 0)));
			normals.push_back(Vec3f(0.0, 0.0, 0.0));
		}
	}

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (i < width - 1)
				springs.push_back(new Spring(ks, kd, gridSize, particles[j * width + i], particles[j * width + i + 1]));
			if (j < height - 1)
				springs.push_back(new Spring(ks, kd, gridSize, particles[j * width + i], particles[(j + 1) * width + i]));
			if (i < width - 1 && j < height - 1) {
				springs.push_back(new Spring(ks, kd, SQRT2 * gridSize, particles[j * width + i], particles[(j + 1) * width + i + 1]));
				springs.push_back(new Spring(ks, kd, SQRT2 * gridSize, particles[j * width + i + 1], particles[(j + 1) * width + i]));
			}
			if (i < width - 2)
				springs.push_back(new Spring(ks, kd, 2 * gridSize, particles[j * width + i], particles[j * width + i + 2]));
			if (j < height - 2)
				springs.push_back(new Spring(ks, kd, 2 * gridSize, particles[j * width + i], particles[(j + 2) * width + i]));
		}
	}
}

void ClothSystem::computeForcesAndUpdateParticles(float t)
{
	if (bake_fps <= 0) return;

	for (auto par : particles)
		par->f = Vec3f(0.0, 0.0, 0.0);

	for (auto spring : springs)
		spring->apply();

	for (auto par : particles)
		updateParticle(par, t, false);

	if (simulate)
		bakeParticles(t);
}

void ClothSystem::drawParticles(float t)
{
	vector<Particle*> particlesToDraw;
	if (baked_data.count(t) > 0) {
		particlesToDraw = baked_data.at(t);
	}
	else if (simulate) {
		if (baked_data.count(t) == 0)
			computeForcesAndUpdateParticles(t);
		particlesToDraw = particles;
	}

	if (!particlesToDraw.empty()) {
		clearNormals();
		for (int j = 0; j < height - 1; j++) {
			for (int i = 0; i < width - 1; i++) {
				Vec3f p0 = particlesToDraw[j * width + i]->p;
				Vec3f p1 = particlesToDraw[(j + 1) * width + i]->p;
				Vec3f p2 = particlesToDraw[(j + 1) * width + i + 1]->p;
				Vec3f p3 = particlesToDraw[j * width + i + 1]->p;

				Vec3f n012 = computeFaceNormal(p0, p1, p2);
				Vec3f n023 = computeFaceNormal(p2, p3, p0);
				Vec3f n013 = computeFaceNormal(p0, p1, p3);
				Vec3f n123 = computeFaceNormal(p1, p2, p3);

				normals[j * width + i] += n012 + n023 + n013;
				normals[(j + 1) * width + i] += n012 + n013 + n123;
				normals[(j + 1) * width + i + 1] += n012 + n023 + n123;
				normals[j * width + i + 1] += n023 + n013 + n123;

				/*normals[j * width + i] += n012;
				normals[(j + 1) * width + i] += n013;
				normals[(j + 1) * width + i + 1] += n123;
				normals[j * width + i + 1] += n023;*/
			}
		}

		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				normals[j * width + i].normalize();
			}
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glEnable(GL_NORMALIZE);

		for (int j = 0; j < height - 1; j++) {
			for (int i = 0; i < width - 1; i++) {
				Vec3f p0 = particlesToDraw[j * width + i]->p;
				Vec3f p1 = particlesToDraw[(j + 1) * width + i]->p;
				Vec3f p2 = particlesToDraw[(j + 1) * width + i + 1]->p;
				Vec3f p3 = particlesToDraw[j * width + i + 1]->p;

				Vec3f n0 = normals[j * width + i];
				Vec3f n1 = normals[(j + 1) * width + i];
				Vec3f n2 = normals[(j + 1) * width + i + 1];
				Vec3f n3 = normals[j * width + i + 1];

				glBegin(GL_TRIANGLE_FAN);
				glNormal3d(n0[0], n0[1], n0[2]);
				glVertex3d(p0[0], p0[1], p0[2]);
				glNormal3d(n1[0], n1[1], n1[2]);
				glVertex3d(p1[0], p1[1], p1[2]);
				glNormal3d(n2[0], n2[1], n2[2]);
				glVertex3d(p2[0], p2[1], p2[2]);
				glNormal3d(n3[0], n3[1], n3[2]);
				glVertex3d(p3[0], p3[1], p3[2]);
				glEnd();
			}
		}
	}
}

Vec3f ClothSystem::computeFaceNormal(Vec3f p0, Vec3f p1, Vec3f p2)
{
	float a, b, c, d, e, f;

	a = p1[0] - p0[0];
	b = p1[1] - p0[1];
	c = p1[2] - p0[2];

	d = p2[0] - p0[0];
	e = p2[1] - p0[1];
	f = p2[2] - p0[2];

	Vec3f result(b * f - c * e, c * d - a * f, a * e - b * d);
	result.normalize();

	return result;
}

void ClothSystem::clearNormals()
{
	for (auto normal : normals)
		normal = Vec3f(0.0, 0.0, 0.0);
}

void ClothSystem::sortParticles()
{
	// do nothing
	return;
}