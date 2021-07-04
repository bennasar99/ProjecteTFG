#include "Sheep.h"

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

class World;

Sheep::Sheep(World* world, Vector3<float> pos): Entity(world, pos, 1.2f)
{
	this->health = 5;
	this->rot = float(rand() % 300);
}

/**
  * Funció d'actualització de l'estat intern del jugador (passiva)
  */
void Sheep::update(double delta) {
	Entity::update(delta);
	rotCounter += (float)delta;
	//printf("rc %f\n", rotCounter);
	if (rotCounter > 5) {
		rotAct = rand() % 5;
		rotCounter = 0;
	}
	//printf("grav %f \n", this->grav);
	if (rotDavDr > 25) {
		rotDR = -1;
	}
	else if (rotDavDr < -25) {
		rotDR = 1;
	}
	rotDavDr += (float)delta * 25 * rotDR;

	//if (Block::isSolid(world->getBlock(front))) {
		//this->rot += delta;
	//}
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
	this->rot = fmod(this->rot, 360.0f);
	Vector3<float> front = Vector3<float>(-sinf(toRad(this->rot)), 0, -cosf(toRad(this->rot)));
	Vector3<float> dir = Vector3<float>::normalize(front);
	this->speed.x = dir.x;
	this->speed.z = dir.z;

	//this->rot = toDegree(Vector3<float>::angle(Vector3<float>(dir.x, 0, dir.z), Vector3<float>(0, 0, 1)));
	//if (dir.x < 0) {
	//	this->rot = 360 - rot; //Conversió de 180 a 360 graus
	//}
	//this->pos = this->pos; /*Vector3<float>(sinf(toRad(this->rot)), 0, sinf(toRad(this->rot)))/100.0f*/;
	//this->pos.x += 0.001f;;
	Vector3<float> newPos = this->pos + dir;
	Bloc nbd = world->getBlock(newPos - Vector3<float>(0, 1.0f, 0));
	Bloc nba = world->getBlock(newPos + Vector3<float>(0, 0, 0));
	Bloc nbu = world->getBlock(newPos + Vector3<float>(0, 1, 0));
	//printf("abaix %d normal %d amunt %d grav %f\n", nbd, nba, nbu, this->grav);
	if (nbu == Bloc::RES && Block::isSolid(nba)) {
		this->grav = -5.0f;
		//newPos = this->pos + Vector3<float>(dir.x, 0, dir.z) * 0.1f + Vector3<float>(0, 1, 0);
	}
	if (this->grav < 0) { //Si tocam adalt, tornam caure
		if (Block::isSolid(nbu)) {
			this->grav = 0;
		}
	}
}


//Funció de dibuix
void Sheep::draw(double delta) {
	glScalef(0.1f, 0.1f, 0.1f);
	TextureManager::applyTexture("Ovella");
	glRotatef(rot, 0, 1, 0);

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
