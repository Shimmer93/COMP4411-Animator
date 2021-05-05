#pragma once

#include "vec.h"
#include <vector>
#include "marchingcube.h"

class Ball {
public:
	Ball(float x, float y, float z, float r) :o({x, y, z}), r(r) {}
	Ball(Vec3f o, float r): o(o), r(r) {}

	Vec3f o;
	float r;
};

class MetaBall
{
public:
	MetaBall(int x, int y, int z, float cubeSize);
	~MetaBall();

	void addBall(Ball ball);
	std::vector<Ball>& getBalls();
	void prepare(int degree);
	void draw(float threshold);

private:
	MarchingCube* marchingCube;
	std::vector<Ball> balls;
};

