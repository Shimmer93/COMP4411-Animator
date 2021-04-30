 // SAMPLE_SOLUTION
#include <FL/gl.h>
#include <GL/glu.h>
#include "particle.h"
#include "modelerdraw.h"
#include "modelerglobals.h"
#include <iostream>
using namespace std;

void Particle::draw(const Vec3f& camPos) {
	if (p[0] > -10 && p[0]<10 && p[1]>-10 && p[1]<10 && p[2]>-10 && p[2] < 10) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, TEXTURE_ID);

		Vec3f objToCamProj, objToCam, upAux;
		Vec3f lookAt(0, 0, 1);
		float angleCos;

		glPushMatrix();
		glTranslatef(p[0], p[1], p[2]);

			glPushMatrix();

			objToCamProj[0] = camPos[0] - p[0];
			objToCamProj[1] = 0;
			objToCamProj[2] = camPos[2] - p[2];
			objToCamProj.normalize();

			upAux = lookAt ^ objToCamProj;
			angleCos = lookAt * objToCamProj;

			if ((angleCos < 0.99990) && (angleCos > -0.9999))
				glRotatef(acosf(angleCos) * 180 / M_PI, upAux[0], upAux[1], upAux[2]);

			objToCam[0] = camPos[0] - p[0];
			objToCam[1] = camPos[1] - p[1];
			objToCam[2] = camPos[2] - p[2];
			objToCam.normalize();

			angleCos = objToCamProj * objToCam;

			if ((angleCos < 0.99990) && (angleCos > -0.9999))
				if (objToCam[1] < 0)
					glRotatef(acosf(angleCos) * 180 / M_PI, 1, 0, 0);
				else
					glRotatef(acosf(angleCos) * 180 / M_PI, -1, 0, 0);

			GLUquadricObj* gluq = gluNewQuadric();
			gluQuadricDrawStyle(gluq, GLU_FILL);
			gluQuadricTexture(gluq, GL_TRUE);
			
			gluDisk(gluq, 0, 0.1, 24, 24);

			glPopMatrix();

		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
	}
}