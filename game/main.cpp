#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include "codegiraffe\v2.h"
#include "codegiraffe\vec3f.h"
#include <GL/glut.h>

using namespace std;

GLuint _textureId;


const float GRAVITY = 5.0f;
const int NUM_PARTICLES = 10000;
const float STEP_TIME = 0.01f;
const float PARTICLE_SIZE = 0.02f;
const int TIMER_MS = 10;

float randomFloat() 
{
	return (float)rand() / ((float)RAND_MAX + 1);
}

struct Particle {
	Vec3f pos;
	Vec3f velocity;
	Vec3f color;
	float timeAlive;
	float lifespan; 
};

class ParticleSystem
{
private:
		GLuint textureId;
		Particle particles[NUM_PARTICLES];
		float timeUntilNextStep;
		float colorTime;
		float angle;

		void createParticle(Particle* p) 
		{
			p->pos = Vec3f(0, 3.0f, 0);
			p->velocity = Vec3f(0.5f * randomFloat() - 0.25f, 0.5f * randomFloat() - 0.25f, 0.5f * randomFloat() - 0.25f);
			p->color = Vec3f(0.0f, 1.0f, 6.0f);
			p->timeAlive = 0;
			p->lifespan = randomFloat() + 1;
		}

		void step() 
		{
			for(int i = 0; i < NUM_PARTICLES; i++) 
			{
				Particle* p = particles + i;
				p->pos += p->velocity * STEP_TIME;
				p->velocity += Vec3f(0.0f, -GRAVITY * STEP_TIME, 0.0f);
				p->timeAlive += STEP_TIME;
				if (p->timeAlive > p->lifespan) {
					createParticle(p);
				}
			}
		}
public:
		ParticleSystem(GLuint m_texture) 
		{
			textureId = m_texture;
			timeUntilNextStep = 0;
			colorTime = 0;
			angle = 0;
			for(int i = 0; i < NUM_PARTICLES; i++) {
				createParticle(particles + i);
			}
			for(int i = 0; i < 5 / STEP_TIME; i++) {
				step();
			}
		}
		
		void advance(float m_dt) 
		{
			while (m_dt > 0) {
				if (timeUntilNextStep < m_dt) {
					m_dt -= timeUntilNextStep;
					step();
					timeUntilNextStep = STEP_TIME;
				}
				else {
					timeUntilNextStep -= m_dt;
					m_dt = 0;
				}
			}
		}
		
		void draw() 
		{
			vector<Particle*> ps;
			for(int i = 0; i < NUM_PARTICLES; i++) 
			{
				ps.push_back(particles + i);
			}
			
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glBegin(GL_QUADS);
			for(unsigned int i = 0; i < ps.size(); i++) 
			{
				Particle* p = ps[i];
				glColor4f(p->color[0], p->color[1], p->color[2],  (1 - p->timeAlive / p->lifespan));
				float size = PARTICLE_SIZE / 2;
				
				Vec3f pos = p->pos;
				
				glTexCoord2f(0, 0);
				glVertex3f(pos[0] - size, pos[1] - size, pos[2]);
				glTexCoord2f(0, 1);
				glVertex3f(pos[0] - size, pos[1] + size, pos[2]);
			}
		glEnd();
	}
};
ParticleSystem* _particles;

void initRendering() 
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void handleResize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void drawScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -10.0f);
	glScalef(2.0f, 2.0f, 2.0f);
	
	_particles->draw();
	
	glutSwapBuffers();
}

void update(int value) 
{
	_particles->advance(TIMER_MS / 1000.0f);
	glutPostRedisplay();
	glutTimerFunc(TIMER_MS, update, 0);
}

int main(int argc, char** argv) 
{
	srand((unsigned int)time(0));
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	
	glutCreateWindow("William Bowden - Partical System Generator - GSP 390");
	initRendering();
	
	glutDisplayFunc(drawScene);
	glutReshapeFunc(handleResize);
	
	_particles = new ParticleSystem(_textureId);
	
	glutTimerFunc(TIMER_MS, update, 0);
	
	glutMainLoop();
	return 0;
}