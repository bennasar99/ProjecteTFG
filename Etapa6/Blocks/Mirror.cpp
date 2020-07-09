#include <GL/glew.h>
#include "Mirror.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

class World;

Mirror::Mirror(World* world, Vector3 pos) : Block (world, Bloc::MIRALL, 0){
    this->pos = pos;

	//Generam el frame buffer, el render buffer i la textura
	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &tfbo);
	glGenRenderbuffers(1, &rbo);

	this->cam = Camera(this->pos + Vector3(0, 0.5f, 0), this->pos + Vector3(-1,0,0));
}

/*
  Funció d'actualització de l'estat intern del mirall, generam l'imatge de la reflexió
*/
void Mirror::update(int delta) {
	glPushMatrix();
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glBindTexture(GL_TEXTURE_2D, tfbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 200, 200, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tfbo, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 200, 200);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("ERROR FRAMEBUFFER\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//-------------------------------------

	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glViewport(0, 0, 200,200); //Establim el viewport del mirall

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(world->camera->getFov(), 1, 1, 10); //I la projecció perspectiva

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	glPushMatrix();
	//Renderitzam el que el mirall ha de mostrar

	Vector3 front = world->camera->getPos() - this->pos;
	front.x = -front.x;
	front.y = -front.y;

	Camera* oldcam = world->camera;
	world->camera = &this->cam; 
	world->camera->setFront(front);
	world->camera->display();
	world->draw(pos, 16); //Món buffer
	world->drawSol(pos, 24);
	world->camera = oldcam;
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //A partir d'aquí mon

	glPopMatrix();

	glEnable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
	glPopMatrix();
}

//Funció de dibuixat, utilitzant la textura generada
void Mirror::draw(bool wireframe) {

	glColor3f(0, 0, 0);
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(-1, 1.5f, 0.0);
	glVertex3f(-1, -0.5f, 0.0);
	glVertex3f(1, -0.5f, 0.0);
	glVertex3f(1, 1.5f, 0.0);
	glEnd();
	glLineWidth(1.0f);

	glColor4f(0,0,0, 0);
	glBegin(GL_QUADS); //Mirall de davant
	glVertex3f(-1, 1.5f, 0);
	glVertex3f(-1, -0.5f, 0);
	glVertex3f(1, -0.5f, 0);
	glVertex3f(1, 1.5f, 0);
	glEnd();

	glColor4f(1, 1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, tfbo); //Activam la textura generada

	glBegin(GL_QUADS); //Mirall de davant
	glTexCoord2f(1, 1);
	glVertex3f(-1, 1.5f, 0.05f);

	glTexCoord2f(1, 0);
	glVertex3f(-1, -0.5f, 0.05f);

	glTexCoord2f(0, 0);
	glVertex3f(1, -0.5f, 0.05f);

	glTexCoord2f(0, 1);
	glVertex3f(1, 1.5f, 0.05f);
	glEnd();

	glBegin(GL_QUADS); //Mirall de darrera
	glTexCoord2f(1, 1);
	glVertex3f(1, 1.5f, -0.05f);

	glTexCoord2f(1, 0);
	glVertex3f(1, -0.5f, -0.05f);

	glTexCoord2f(0, 0);
	glVertex3f(-1, -0.5f, -0.05f);

	glTexCoord2f(0, 1);
	glVertex3f(-1, 1.5f, -0.05f);
	glEnd();
}

//Eliminam els buffers i la textura
void Mirror::destroy() {
	glDeleteTextures(1, &tfbo);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &fbo);
}