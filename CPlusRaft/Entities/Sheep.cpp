#include "Sheep.h"

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

class World;

Sheep::Sheep(World* world, Vector3<float> pos): Mob(world, pos, 1.2f)
{
	this->health = 5;
	this->rot = float(rand() % 300);
}

/**
  * Funció d'actualització de l'estat intern del jugador (passiva)
  */
void Sheep::update(double delta) {
	Mob::update(delta);
	float offset = this->height / 2.0f;
	//printf("grav %f \n", this->grav);
	if (rotDavDr > 25) {
		rotDR = -1;
	}
	else if (rotDavDr < -25) {
		rotDR = 1;
	}
	rotDavDr += (float)delta * 25 * rotDR;

	switch (rotAct) {
	case 1:
		this->rot += (float)delta*25;
		break;
	case 2: 
		this->rot -= (float)delta*25;
		break;
	default:
		break;
	}
	
}


//Funció de dibuix
void Sheep::draw(double delta) {
	glScalef(0.1f, 0.1f, 0.1f);
	TextureManager::applyTexture("Ovella");
	glRotatef(rot + 180, 0, 1, 0);

	glPushMatrix();
	glTranslatef(-0, 1, 0);
	ModelManager::drawModel("OvellaCos", 0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.0f, -2.5f, 5.5f);
	glRotatef(-rotDavDr, 1, 0, 0);
	ModelManager::drawModel("OvellaCamaDavantDreta", 0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(+3.0f, -2.5f, 5.5f);
	glRotatef(rotDavDr, 1, 0, 0);
	ModelManager::drawModel("OvellaCamaDavantEsquerra", 0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(+3.0f, -2.5f, -5.5f);
	glRotatef(rotDavDr, 1, 0, 0);
	ModelManager::drawModel("OvellaCamaDarreraEsquerra", 0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.0f, -2.5f, -5.5f);
	glRotatef(-rotDavDr, 1, 0, 0);
	ModelManager::drawModel("OvellaCamaDarreraDreta", 0);
	glPopMatrix();

	TextureManager::noTexture();
}

void Sheep::destroy() {
}

void Sheep::setCam(Camera* cam) {
	cam->setPos(this->pos + Vector3<float>(0, eyesOffset, 0));
}
