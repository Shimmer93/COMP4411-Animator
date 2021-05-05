#pragma once
#include "particleSystem.h"

class ClothSystem : public ParticleSystem
{
public:
	ClothSystem(vector<Force*> forces, float gridSize, int height, int width, Vec3f startPoint, float ks, float kd);

	void drawParticles(float t);
	void computeForcesAndUpdateParticles(float t);

private:
	vector<Spring*> springs;
	vector<Vec3f> normals;
	float gridSize;
	int height, width;

	Vec3f computeFaceNormal(Vec3f p0, Vec3f p1, Vec3f p2);
	void clearNormals();
	void sortParticles();
};

