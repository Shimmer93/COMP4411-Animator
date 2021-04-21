#include "metaball.h"

MetaBall::MetaBall(int x, int y, int z, double cubeSize) 
	:marchingCube(new MarchingCube(x, y, z, cubeSize)), balls()
{}

MetaBall::~MetaBall() {
	delete marchingCube;
}

void MetaBall::addBall(Ball ball) {
	balls.push_back(ball);
}

void MetaBall::prepare(int degree) {
	if (degree <= 0) return;

	for (Ball ball : balls) {
		double ox = ball.o[0];
		double oy = ball.o[1];
		double oz = ball.o[2];
		double r = ball.r;

		for (int i = 0; i < marchingCube->getNumVertex(); i++) {
			Vertex& v = marchingCube->vertices[i];
			double x = v.point[0];
			double y = v.point[1];
			double z = v.point[2];
			// square of distance
			double d2 = pow(ox - x, degree) + pow(oy - y, degree) + pow(oz - z, degree) + 1e-8;
			// factor to calculate the gradient
			double gd = -1.0 * degree * r * r / (pow(ox - x, degree + 2) + pow(oy - y, degree + 2) + pow(oz - z, degree + 2) + 1e-8);

			// update the value and normal
			v.value += r * r / d2;
			v.normal[0] += gd * (ox - x);
			v.normal[1] += gd * (oy - y);
			v.normal[2] += gd * (oz - z);
		}
	}
}

void MetaBall::draw(double threshold) {
	marchingCube->draw(threshold);
}