#include "Sheep.h"

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

class World;

Sheep::Sheep(World* world, Vector3<float> pos): Entity(world, pos)
{
	cos = Mesh("Models/Sheep/sheep.obj");
	davDR = Mesh("Models/Sheep/SheepDavDR.obj");
	davES = Mesh("Models/Sheep/SheepDavES.obj");
	darDR = Mesh("Models/Sheep/SheepDarDR.obj");
	darES = Mesh("Models/Sheep/SheepDarES.obj");
	this->health = 5;
	this->firstdraw = true;
}

/**
  * Funci� d'actualitzaci� de l'estat intern del jugador (passiva)
  */
void Sheep::update(float delta) {
	if (rotDavDr > 25) {
		rotDR = -1;
	}
	else if (rotDavDr < -25) {
		rotDR = 1;
	}
	rotDavDr += delta * 25 * rotDR;

	Vector3<float> front = this->pos + Vector3<float>(sinf(this->rot), 0, cosf(this->rot)) * 2;
	if (Block::isSolid(world->getBlock(front))) {
		//this->rot += delta;
	}
	this->rot += delta*10;
	this->pos = this->pos + Vector3<float>(sinf(toRad(this->rot)), 0, sinf(toRad(this->rot)))/100.0f;
	//this->pos.x += 0.001f;;
	//Gravetat
	Bloc bd = world->getBlock(this->pos - Vector3<float>(0, 1, 0));
	Bloc ba = world->getBlock(this->pos);
	if (this->grav > 0 && Block::isSolid(bd) && ba != Bloc::AIGUA ) {
		this->grav = 0;
	}
	else if (this->grav < gravmax && (bd == Bloc::RES || bd == Bloc::LIMIT)) { //"Gravetat"
		this->grav += delta * 9.8f;
	}
	Vector3<float> newPos = this->pos + Vector3<float>(0, -1, 0) * delta * this->grav;
	if (!Block::isSolid(world->getBlock(newPos - Vector3<float>(0, 1, 0))) && this->grav >= 0.0f) { //Caiem
		this->pos = newPos;
	}
	if (this->grav < 0) { //Si tocam adalt, tornam caure
		if (Block::isSolid(world->getBlock(this->pos + Vector3<float>(0, eyesOffset + 0.2f, 0)))) {
			this->grav = 0;
		}
	}

	if (ba == Bloc::AIGUA) {
		if (this->grav > -0.1f) {
			this->grav += -0.1f;
		}
	}

	//Bot
}


//Funci� de dibuix
void Sheep::draw() {
	if (firstdraw) {
		firstdraw = false;
		cos.update();
		davDR.update();
		davES.update();
		darDR.update();
		darES.update();
	}
	glScalef(0.1f, 0.1f, 0.1f);
	glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::OVELLA));
	glRotatef(rot, 0, 1, 0);
	cos.draw();

	glPushMatrix();
	glTranslatef(-3.0f, +5.5, 5.5f);
	glRotatef(rotDavDr, 1, 0, 0);
	davDR.draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(+3.0f, +5.5, 5.5f);
	glRotatef(rotDavDr, 1, 0, 0);
	davES.draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(+3.0f, +5.5, -5.5f);
	glRotatef(rotDavDr, 1, 0, 0);
	darES.draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.0f, +5.5, -5.5f);
	glRotatef(rotDavDr, 1, 0, 0);
	darDR.draw();
	glPopMatrix();

	/*davES.draw();
	darDR.draw();
	darES.draw();*/
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Sheep::destroy() {
}

void Sheep::setCam(Camera* cam) {
	cam->setPos(this->pos + Vector3<float>(0, eyesOffset, 0));
}
