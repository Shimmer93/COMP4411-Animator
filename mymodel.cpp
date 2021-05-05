// The sample model.  You should build a file
// very similar to this for when you make your model.
#include "bitmap.h"
#include "tga.h"
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "modelerui.h"
#include <FL/gl.h>
#include <cmath>
#include <functional>
#include <vector>

#include "modelerglobals.h"
#include "metaball.h"
#include "particleSystem.h"
#include "mat.h"
#include "force.h"
#include "ClothSystem.h"

#include <iostream>
using namespace std;

int FRAME = 0;
int MOOD_COUNT = 0;
const int MAX_FRAME = 60;
const int MAX_MOOD_COUNT = 60;
bool MOOD = false;

// To make a SampleModel, we inherit off of ModelerView
class MyModel : public ModelerView
{
public:
	MyModel(int x, int y, int w, int h, char* label)
		: ModelerView(x, y, w, h, label), initialized(false), metaBall(new MetaBall(50, 50, 50, 0.2))
	{
		vector<Force*> forces;
		forces.push_back(new TestForce());
		cloth = new ClothSystem(forces, ModelerApplication::Instance()->GetFps(),
			0.1, 50, 50, Vec3f(-2.5, -2.5, 0), 2.0, 0.5);

		collision = new ParticleSystem(vector<Force*>(), ModelerApplication::Instance()->GetFps(), true);
		collision->addParticle(new Particle(0.1, Vec3f(-2, -2, 0), Vec3f(1, 1, 0), Vec3f(0, 0, 0)));
		collision->addParticle(new Particle(0.2, Vec3f(2, 1, 0), Vec3f(-1, -0.5, 0), Vec3f(0, 0, 0)));
	}

	void init();
	virtual void draw();

private:
	bool initialized;
	MetaBall* metaBall;
	ClothSystem* cloth;
	ParticleSystem* collision;

	void drawArmWithShoulder(int angle, double x, double y, double z);
	void drawOriginal();
	void drawCloth();
	void drawCollision();
	void drawMetaball();
};

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createMyModel(int x, int y, int w, int h, char* label)
{
	return new MyModel(x, y, w, h, label);
}

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out SampleModel

Mat4f getModelViewMatrix()
{
	/**************************
	**
	**	GET THE OPENGL MODELVIEW MATRIX
	**
	**	Since OpenGL stores it's matricies in
	**	column major order and our library
	**	use row major order, we will need to
	**	transpose what OpenGL gives us before returning.
	**
	**	Hint:  Use look up glGetFloatv or glGetDoublev
	**	for how to get these values from OpenGL.
	**
	*******************************/

	GLfloat m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	Mat4f matMV(m[0], m[1], m[2], m[3],
				m[4], m[5], m[6], m[7],
				m[8], m[9], m[10], m[11],
				m[12], m[13], m[14], m[15]);

	return matMV.transpose(); // convert to row major
}

// Util function for animation
float getCurrentFrameArmAngle() {
	if (FRAME < MAX_FRAME / 4) 
		return FRAME;
	else if (FRAME < MAX_FRAME * 3 / 4) 
		return MAX_FRAME / 2 - FRAME;
	else 
		return FRAME - MAX_FRAME;
}

float getCurrentFrameBodyHeight() {
	if (FRAME < MAX_FRAME / 4)
		return -0.01 * FRAME;
	else if (FRAME < MAX_FRAME / 2)
		return -0.01 * (MAX_FRAME / 2 - FRAME);
	else if (FRAME < MAX_FRAME * 3 / 4)
		return -0.01 * (FRAME - MAX_FRAME / 2);
	else
		return -0.01 * (MAX_FRAME - FRAME);
}

// Initialize the texture
void initTexture() {
	// Read the bitmap
	char* imgName = "textures/Bubble.tga";
	int width, height;

	unsigned char* data = readTGA(imgName, width, height);

	// Load the bitmap into the texture
	GLuint textureID;
	glGenTextures(TEXTURE_ID, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Set parameters for the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Tricky implementation od surface of rotation, using drawCylinder()
void drawSurfaceOfRotation(double h, std::function<double(double)> curve) {

	ModelerDrawState* mds = ModelerDrawState::Instance();
	int divisions;

	switch (mds->m_quality)
	{
	case HIGH:
		divisions = 32; break;
	case MEDIUM:
		divisions = 20; break;
	case LOW:
		divisions = 12; break;
	case POOR:
		divisions = 8; break;
	}

	double delta = h / divisions;
	double currentR, prevR;
	int count = 0;
	for (double z = delta; count < divisions; z += delta, count++) {
		if (z == delta) prevR = curve(0);
		else prevR = currentR;
		currentR = curve(z);
		glPushMatrix();
		glTranslated(0.0, 0.0, z - delta);
		drawCylinder(delta, prevR, currentR);
		glPopMatrix();
	}
}

// Ugly implementation of L-system
void drawBinaryLSystem(int numIter, std::function<void(int)> drawStart) {
	drawStart(numIter);
}

void drawB(int numIter);

void drawA(int numIter) {
	drawCylinder(0.3 * numIter, 0.15 * numIter, 0.15 * numIter);

	if (numIter > 0) {
		glPushMatrix();
		glTranslated(0.0, 0.0, 0.3 * numIter);
		drawA(numIter - 1);
		drawB(numIter);
		glPopMatrix();
	}
}

void drawB(int numIter) {
	glPushMatrix();
	glTranslated(0.1 * numIter, 0.0, -0.15 * numIter);
	glRotated(90, 0.0, 1.0, 0.0);
	drawCylinder(0.3 * numIter, 0.06 * numIter, 0);
	glPopMatrix();
	
	if (numIter > 0) {
		glPushMatrix();
		glTranslated(0.08 * numIter, 0.0, -0.12 * numIter);
		glRotated(-1.0 * VAL(L_SYSTEM_ANGLE), 0.0, 1.0, 0.0);
		drawB(numIter - 1);
		glPopMatrix();
	}

}

// Separate the arm part since Metagross has four arms
void MyModel::drawArmWithShoulder(int angle, double x, double y, double z) {
	int ARM_ROTATE_X = 0;
	int ARM_ROTATE_Z = 0;
	switch (angle) {
	case 45:
		if (!MOOD) {
			ARM_ROTATE_X = VAL(FRONT_LEFT_ROTATE_X) - getCurrentFrameArmAngle();
			ARM_ROTATE_Z = VAL(FRONT_LEFT_ROTATE_Z);
		}
		else {
			ARM_ROTATE_X = VAL(FRONT_LEFT_ROTATE_X);
			ARM_ROTATE_Z = VAL(FRONT_LEFT_ROTATE_Z) - getCurrentFrameArmAngle();
		}
		break;
	case 90:
		if (!MOOD) {
			ARM_ROTATE_X = VAL(LEFT_ROTATE_X) - getCurrentFrameArmAngle();
			ARM_ROTATE_Z = VAL(LEFT_ROTATE_Z);
		}
		else {
			ARM_ROTATE_X = VAL(LEFT_ROTATE_X);
			ARM_ROTATE_Z = VAL(LEFT_ROTATE_Z) - getCurrentFrameArmAngle();
		}
		break;
	case 135:
		if (!MOOD) {
			ARM_ROTATE_X = VAL(BACK_LEFT_ROTATE_X) - getCurrentFrameArmAngle();
			ARM_ROTATE_Z = VAL(BACK_LEFT_ROTATE_Z);
		}
		else {
			ARM_ROTATE_X = VAL(BACK_LEFT_ROTATE_X);
			ARM_ROTATE_Z = VAL(BACK_LEFT_ROTATE_Z) - getCurrentFrameArmAngle();
		}
		break;
	case 225:
		if (!MOOD) {
			ARM_ROTATE_X = VAL(BACK_RIGHT_ROTATE_X) + getCurrentFrameArmAngle();
			ARM_ROTATE_Z = VAL(BACK_RIGHT_ROTATE_Z);
		}
		else {
			ARM_ROTATE_X = VAL(BACK_RIGHT_ROTATE_X);
			ARM_ROTATE_Z = VAL(BACK_RIGHT_ROTATE_Z) + getCurrentFrameArmAngle();
		}
		break;
	case 270:
		if (!MOOD) {
			ARM_ROTATE_X = VAL(RIGHT_ROTATE_X) + getCurrentFrameArmAngle();
			ARM_ROTATE_Z = VAL(RIGHT_ROTATE_Z);
		}
		else {
			ARM_ROTATE_X = VAL(RIGHT_ROTATE_X);
			ARM_ROTATE_Z = VAL(RIGHT_ROTATE_Z) + getCurrentFrameArmAngle();
		}
		break;
	case 315:
		if (!MOOD) {
			ARM_ROTATE_X = VAL(FRONT_RIGHT_ROTATE_X) + getCurrentFrameArmAngle();
			ARM_ROTATE_Z = VAL(FRONT_RIGHT_ROTATE_Z);
		}
		else {
			ARM_ROTATE_X = VAL(FRONT_RIGHT_ROTATE_X);
			ARM_ROTATE_Z = VAL(FRONT_RIGHT_ROTATE_Z) + getCurrentFrameArmAngle();
		}
		break;
	default:
		ARM_ROTATE_X = 0;
		ARM_ROTATE_Z = 0;
	}

	//layer 2: arm with shoulder
	setDiffuseColor(0.616, 0.753, 0.816);
	glPushMatrix();
	glTranslated(x, y, z);
	glRotated(ARM_ROTATE_X, 1.0, 0.0, 0.0);
	glRotated(ARM_ROTATE_Z, 0.0, 0.0, 1.0);
	glRotated(angle, 0.0, 1.0, 0.0);

		// layer 3: shoulder
		glPushMatrix();
			// layer 4: ball in shoulder
			glPushMatrix();
			drawSphere(0.3);
			glPopMatrix();

			// layer 4: cylinder in shoulder
			glPushMatrix();
			drawCylinder(VAL(SHOULDER_LENGTH), 0.1, 0.1);
			glPopMatrix();
		glPopMatrix();

		// layer 3: arm
		glPushMatrix();
		glRotated(270, 1.0, 0.0, 0.0);
		glTranslated(0.0, -1.0 * VAL(SHOULDER_LENGTH), -1.0*(0.5*VAL(ARM_LENGTH)+0.35));
		glTranslated(0.0, 0.0, (0.5 * VAL(ARM_LENGTH) + 0.35));
		glRotated(VAL(ARM_VERTICAL_ANGLE), 1.0, 0.0, 0.0);
		glTranslated(0.0, 0.0, -1.0*(0.5 * VAL(ARM_LENGTH) + 0.35));
		glRotated(-90, 0.0, 0.0, 1.0);

			// layer 4: main part cylinder
			glPushMatrix();
			if (VAL(USE_SURFACE_OF_ROTATION) == 1) {
				drawSurfaceOfRotation(VAL(ARM_LENGTH), [](double z) -> double { return 0.6 - 0.04115 * z * z; });
			}
			else if (VAL(USE_COMPLEX_SHAPE) == 1){
				for (int i = 0; i < 6; i++) {
					drawTriangle(0.6 * cos(M_PI * i / 3), 0.6 * sin(M_PI * i / 3), 0.0, 
								   0.6 * cos(M_PI * (i + 1) / 3), 0.6 * sin(M_PI * (i + 1) / 3), 0.0, 
								   0.3 * cos(M_PI * (2 * i + 1) / 6), 0.3 * sin(M_PI * (2 * i + 1) / 6), VAL(ARM_LENGTH));
					drawTriangle(0.3 * cos(M_PI * (2 * i + 1) / 6), 0.3 * sin(M_PI * (2 * i + 1) / 6), VAL(ARM_LENGTH),
								   0.3 * cos(M_PI * (2 * i - 1) / 6), 0.3 * sin(M_PI * (2 * i - 1) / 6), VAL(ARM_LENGTH), 
								   0.6 * cos(M_PI * i / 3), 0.6 * sin(M_PI * i / 3), 0.0);
					drawTriangle(0.0, 0.0, VAL(ARM_LENGTH),
								   0.3 * cos(M_PI * (2 * i - 1) / 6), 0.3 * sin(M_PI * (2 * i - 1) / 6), VAL(ARM_LENGTH),
								   0.3 * cos(M_PI * (2 * i + 1) / 6), 0.3 * sin(M_PI * (2 * i + 1) / 6), VAL(ARM_LENGTH));
				}
			}
			else if (VAL(USE_L_SYSTEM) == 1) {
				drawBinaryLSystem(VAL(L_SYSTEM_NUM_ITER), drawA);
			}
			else if (VAL(USE_METABALL) == 1) {
				glPushMatrix();
				glTranslated(-1.0, -1.0, 0.0);
				metaBall->draw(VAL(METABALL_THRESHOLD));
				glPopMatrix();
			}
			else {
				drawCylinder(VAL(ARM_LENGTH), 0.6, 0.3);
			}
			glPopMatrix();

			// layer 4: lower part cylinder
			glPushMatrix();
			glTranslated(0.0, 0.0, -0.3);
			drawCylinder(0.3, 0.7, 0.7);
			glPopMatrix();

			// layer 4: fingers
			glPushMatrix();
			glRotated(VAL(FINGERS_ROTATE), 0.0, 0.0, 1.0);

				// layer 5: finger 1
				setDiffuseColor(0.8, 0.8, 0.8);
				glPushMatrix();
				glTranslated(0.0, 0.4, -0.6);
				drawCylinder(0.3, 0.0, 0.2);
				glPopMatrix();
				// layer 5: finger 2
				glPushMatrix();
				glTranslated(0.0, -0.4, -0.6);
				drawCylinder(0.3, 0.0, 0.2);
				glPopMatrix();
				// layer 5: finger 3
				glPushMatrix();
				glTranslated(0.4, 0.0, -0.6);
				drawCylinder(0.3, 0.0, 0.2);
				glPopMatrix();
				// layer 5: finger 4
				glPushMatrix();
				glTranslated(-0.4, 0.0, -0.6);
				drawCylinder(0.3, 0.0, 0.2);
				glPopMatrix();

			glPopMatrix();

		glPopMatrix();

	glPopMatrix();
}

// Run once before start to draw
void MyModel::init()
{
	initTexture();

	metaBall->addBall(Ball(1.0, 1.0, 0.6, 0.6));
	metaBall->addBall(Ball(1.0, 1.0, 1.4, 0.4));
	metaBall->addBall(Ball(1.0, 1.0, 2.0, 0.3));
	metaBall->addBall(Ball(1.0, 1.0, 2.4, 0.2));
	metaBall->prepare(2);

	initialized = true;
}

void MyModel::draw()
{
	// This call takes care of a lot of the nasty projection 
	// matrix stuff.  Unless you want to fudge directly with the 
	// projection matrix, don't bother with this ...
	ModelerView::draw();

	// Initialization
	if (!initialized)
		init();

	// Texture
	if (VAL(USE_TEXTURE) == 1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, TEXTURE_ID);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}

	// Animation
	bool isAnimated = false;//ModelerUserInterface::m_controlsAnimOnMenu->value();
	if (!isAnimated) {
		FRAME = 0;
		MOOD_COUNT = 0;
		MOOD = false;
	}

	// layer 1: whole model
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(0.616, 0.753, 0.816);

	// Lighting
	float light0pos[4] = {VAL(LIGHT0_X), VAL(LIGHT0_Y), VAL(LIGHT0_Z), 0.0};
	float light1pos[4] = {VAL(LIGHT1_X), VAL(LIGHT1_Y), VAL(LIGHT1_Z), 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
	glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
	if (VAL(USE_SPECULAR) == 1) {
		float light0spec[4] = { 1.0, 1.0, 1.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_SPECULAR, light0spec);
		setSpecularColor(1.0, 1.0, 1.0);
		setShininess(VAL(SPECULAR_SHINESS));
	}
	else {
		float light0spec[4] = { 0.0, 0.0, 0.0, 0.0 };
		glLightfv(GL_LIGHT0, GL_SPECULAR, light0spec);
	}
	
	switch (ModelerApplication::Instance()->GetUI()->getDrawMode()) {
	case ORIGINAL:
		drawOriginal();
		break;
	case CLOTH:
		drawCloth();
		break;
	case COLLISION:
		drawCollision();
		break;
	case METABALL:
		drawMetaball();
		break;
	default:
		break;
	}

	if (isAnimated) {
		FRAME++;
		FRAME %= MAX_FRAME;
		if (VAL(MOOD_CYCLING) == 1) {
			MOOD_COUNT++;
			if (MOOD_COUNT >= MAX_MOOD_COUNT) {
				MOOD = (MOOD) ? false : true;
				MOOD_COUNT %= MAX_MOOD_COUNT;
			}
		}
		else {
			MOOD = false;
			MOOD_COUNT = 0;
		}
	}
}

void MyModel::drawOriginal()
{
	
	Mat4f cameraMatrix = getModelViewMatrix();
	ParticleSystem* ps = ModelerApplication::Instance()->GetParticleSystem();

	glPushMatrix();
	glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
	glScaled(VAL(XSCALE), VAL(YSCALE), VAL(ZSCALE));

	// layer 2: main body with cross
	glPushMatrix();
	glRotated(VAL(BODY_ROTATE_X), 1.0, 0.0, 0.0);
	glRotated(VAL(BODY_ROTATE_Y), 0.0, 1.0, 0.0);
	glRotated(VAL(BODY_ROTATE_Z), 0.0, 0.0, 1.0);
	glTranslated(0.0, getCurrentFrameBodyHeight(), 0.0);
	glTranslated(0.0, 0.0, 1.0);
	glPushMatrix();
	glRotated(270, 1.0, 0.0, 0.0);
	// layer 4: middle cylinder
	glPushMatrix();
	glTranslated(0.0, 1.0, 0.0);
	drawCylinder(0.5, 2.0, 2.0);
	glPopMatrix();

	// layer 4: upper cylinder
	glPushMatrix();
	glTranslated(0.0, 1.0, 0.5);
	drawCylinder((VAL(BODY_HEIGHT) - 0.5) * 5.0 / 9.0, 1.8, 1.2);
	glPopMatrix();

	Mat4f modelViewMatrix = getModelViewMatrix();
	ps->spawnParticles(cameraMatrix, modelViewMatrix, 10);

	// layer 4: lower cylinder
	glPushMatrix();
	glTranslated(0.0, 1.0, (VAL(BODY_HEIGHT) - 0.5) * (-4.0) / 9.0);
	drawCylinder((VAL(BODY_HEIGHT) - 0.5) * 4.0 / 9.0, 1.2, 1.8);
	glPopMatrix();
	glPopMatrix();

	// layer 3: cross
	glPushMatrix();
	setDiffuseColor(0.8, 0.8, 0.8);

	// layer 4: one stroke of cross
	glPushMatrix();
	glTranslated(-1.2, -0.4, 1.0);
	glRotated(30, 0.0, 0.0, 1.0);
	drawBox(2.4, 0.4, 0.2);
	glPopMatrix();

	// layer 4: another stroke of cross
	glPushMatrix();
	glTranslated(-1.2, 0.8, 1.0);
	glRotated(-30, 0.0, 0.0, 1.0);
	drawBox(2.4, 0.4, 0.2);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	//layer 2: arms with shoulders
	drawArmWithShoulder(45, 1.5, 1.2, 1.0);
	drawArmWithShoulder(135, 1.5, 1.2, -1.0);
	drawArmWithShoulder(225, -1.5, 1.2, -1.0);
	drawArmWithShoulder(315, -1.5, 1.2, 1.0);
	if (VAL(ARM_NUMBER) == 5)
		drawArmWithShoulder(90, 1.5, 1.2, 0);
	if (VAL(ARM_NUMBER) == 6) {
		drawArmWithShoulder(90, 1.5, 1.2, 0);
		drawArmWithShoulder(270, -1.5, 1.2, 0);
	}

	ps->drawParticles(ModelerApplication::Instance()->GetTime());
	glPopMatrix();

	if (ModelerApplication::Instance()->GetTime() == 0)
		ps->clearParticles();
}

void MyModel::drawCloth()
{
	if (ModelerApplication::Instance()->GetParticleSystem() != cloth) {
		ModelerApplication::Instance()->SetParticleSystem(cloth);
		cloth->setCamera(ModelerApplication::Instance()->GetUI()->m_pwndModelerView->m_camera);
	}
	cloth->drawParticles(ModelerApplication::Instance()->GetTime());
}

void MyModel::drawCollision()
{
	if (ModelerApplication::Instance()->GetParticleSystem() != collision) {
		ModelerApplication::Instance()->SetParticleSystem(collision);
		collision->setCamera(ModelerApplication::Instance()->GetUI()->m_pwndModelerView->m_camera);
	}
	collision->drawParticles(ModelerApplication::Instance()->GetTime());
}

void MyModel::drawMetaball()
{
	return;
}

int main()
{
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
	ModelerControl controls[NUMCONTROLS];
	controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1, 0);
	controls[YPOS] = ModelerControl("Y Position", 0, 5, 0.1, 0);
	controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1, 0);
	controls[XSCALE] = ModelerControl("X Scale", 0.5, 2.0, 0.1, 1.2);
	controls[YSCALE] = ModelerControl("Y Scale", 0.5, 2.0, 0.1, 1.2);
	controls[ZSCALE] = ModelerControl("Z Scale", 0.5, 2.0, 0.1, 1.2);
	controls[BODY_HEIGHT] = ModelerControl("Body height", 1.5, 3.1, 0.1, 2.3);
	controls[ARM_LENGTH] = ModelerControl("Arm length", 1.5, 3.5, 0.1, 2.7);
	controls[SHOULDER_LENGTH] = ModelerControl("Shoulder length", 0.8, 2.0, 0.1, 1.0);
	controls[ARM_VERTICAL_ANGLE] = ModelerControl("Arm Vertical Angle", -45, 15, 1, -20);
	controls[FINGERS_ROTATE] = ModelerControl("Fingers Rotation", 0, 90, 1, 0);
	controls[FRONT_LEFT_ROTATE_X] = ModelerControl("Front Left Arm Rotation X", -45, 45, 1, 0);
	controls[LEFT_ROTATE_X] = ModelerControl("Left Arm Rotation X (when >= 5 arms)", -45, 45, 1, 0);
	controls[BACK_LEFT_ROTATE_X] = ModelerControl("Back Left Arm Rotation X", -45, 45, 1, 0);
	controls[BACK_RIGHT_ROTATE_X] = ModelerControl("Back Right Arm Rotation X", -45, 45, 1, 0);
	controls[RIGHT_ROTATE_X] = ModelerControl("Right Arm Rotation X (when 6 arms)", -45, 45, 1, 0);
	controls[FRONT_RIGHT_ROTATE_X] = ModelerControl("Front Right Arm Rotation X", -45, 45, 1, 0);
	controls[FRONT_LEFT_ROTATE_Z] = ModelerControl("Front Left Arm Rotation Z", -45, 45, 1, 0);
	controls[LEFT_ROTATE_Z] = ModelerControl("Left Arm Rotation Z (when >= 5 arms)", -45, 45, 1, 0);
	controls[BACK_LEFT_ROTATE_Z] = ModelerControl("Back Left Arm Rotation Z", -45, 45, 1, 0);
	controls[BACK_RIGHT_ROTATE_Z] = ModelerControl("Back Right Arm Rotation Z", -45, 45, 1, 0);
	controls[RIGHT_ROTATE_Z] = ModelerControl("Right Arm Rotation Z (when 6 arms)", -45, 45, 1, 0);
	controls[FRONT_RIGHT_ROTATE_Z] = ModelerControl("Front Right Arm Rotation Z", -45, 45, 1, 0);
	controls[BODY_ROTATE_X] = ModelerControl("Body Rotation X", -15, 15, 1, 0);
	controls[BODY_ROTATE_Y] = ModelerControl("Body Rotation Y", -15, 15, 1, 0);
	controls[BODY_ROTATE_Z] = ModelerControl("Body Rotation Z", -15, 15, 1, 0);
	controls[LIGHT0_X] = ModelerControl("Light0 Position X", -10, 10, 1, 4);
	controls[LIGHT0_Y] = ModelerControl("Light0 Position Y", -10, 10, 1, 2);
	controls[LIGHT0_Z] = ModelerControl("Light0 Position Z", -10, 10, 1, -4);
	controls[LIGHT1_X] = ModelerControl("Light1 Position X", -10, 10, 1, -2);
	controls[LIGHT1_Y] = ModelerControl("Light1 Position Y", -10, 10, 1, 1);
	controls[LIGHT1_Z] = ModelerControl("Light1 Position Z", -10, 10, 1, 5);
	controls[USE_SPECULAR] = ModelerControl("Use Light0 Specular?", 0, 1, 1, 0);
	controls[SPECULAR_SHINESS] = ModelerControl("Light0 Specular Shiness", 1, 40, 1, 10);
	controls[LEVEL_DETAIL] = ModelerControl("Level of Details", 1, 3, 1, 3);
	controls[USE_TEXTURE] = ModelerControl("Use Texture?", 0, 1, 1, 0);
	controls[USE_COMPLEX_SHAPE] = ModelerControl("Use Complex Shape?", 0, 1, 1, 0);
	controls[USE_TWIST] = ModelerControl("Use Twist Instead of Zoom?", 0, 1, 1, 0);
	controls[USE_SURFACE_OF_ROTATION] = ModelerControl("Use Surface of Rotation?", 0, 1, 1, 0);
	controls[USE_L_SYSTEM] = ModelerControl("Use L System?", 0, 1, 1, 0);
	controls[L_SYSTEM_NUM_ITER] = ModelerControl("L System Number of Iteration", 3, 5, 1, 4);
	controls[L_SYSTEM_ANGLE] = ModelerControl("L System Angle", 30, 60, 1, 45);
	controls[USE_METABALL] = ModelerControl("Use Metaball?", 0, 1, 1, 0);
	controls[METABALL_THRESHOLD] = ModelerControl("Metaball Threshold", 0.1, 5.0, 0.1, 1.0);
	controls[ARM_NUMBER] = ModelerControl("Amount of Arms", 4, 6, 1, 4);
	controls[MOOD_CYCLING] = ModelerControl("Enable Mood Cycling?", 0, 1, 1, 0);

	ModelerApplication::Instance()->Init(&createMyModel, controls, NUMCONTROLS);

	vector<Force*> forces;
	forces.push_back(new Gravity(5.0));
	ParticleSystem* ps = new ParticleSystem(forces, ModelerApplication::Instance()->GetFps(), false);
	ps->setCamera(ModelerApplication::Instance()->GetUI()->m_pwndModelerView->m_camera);

	ModelerApplication::Instance()->SetParticleSystem(ps);
	return ModelerApplication::Instance()->Run();
}
