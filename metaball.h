#pragma once

#include "vec.h"
#include <vector>
#include "marchingcube.h"

class Ball {
public:
	Ball(double x, double y, double z, double r) :o({x, y, z}), r(r) {}
	Ball(Vec3d o, double r): o(o), r(r) {}

	Vec3d o;
	double r;
};

class MetaBall
{
public:
	MetaBall(int x, int y, int z, double cubeSize);
	~MetaBall();

	void addBall(Ball ball);
	void prepare(int degree);
	void draw(double threshold);

private:
	MarchingCube* marchingCube;
	std::vector<Ball> balls;
};

