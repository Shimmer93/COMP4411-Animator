 // SAMPLE_SOLUTION
#include "particle.h"
#include "modelerdraw.h"
#include <FL/gl.h>

void Particle::draw() {
	if (p[0] > -10 && p[0]<10 && p[1]>-10 && p[1]<10 && p[2]>-10 && p[2] < 10) {
		glPushMatrix();
		glTranslatef(p[0], p[1], p[2]);
		drawSphere(0.1);
		glPopMatrix();
	}
}