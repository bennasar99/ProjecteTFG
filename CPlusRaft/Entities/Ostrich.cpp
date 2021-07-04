#include "Ostrich.h"

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

class World;

Ostrich::Ostrich(World* world, Vector3<float> pos): Entity(world, pos, 2.2f)
{
	this->health = 5;
	this->estat = Estat::CAMINANT;
}

/**
  * Funció d'actualització de l'estat intern del jugador (passiva)
  */
void Ostrich::update(double delta) {
	Entity::update(delta);
	printf("gravO %f\n", this->grav);
	rotCounter += (float)delta;
	//printf("rc %f\n", rotCounter);
	if (rotCounter > 5) {
		rotAct = rand() % 5;
		//printf("ra %d", rotAct);
		rotCounter = 0;
	}

	//if (Block::isSolid(world->getBlock(front))) {
		//this->rot += delta;
	//}
	bool canviEstat = false;
	switch (rotAct) {
	case 1:
		if (this->estat == Estat::CAMINANT || this->estat == Estat::ATURAT) {
			this->rot += (float)delta * 25;
		}
		break;
	case 2:
		if (this->estat == Estat::CAMINANT || this->estat == Estat::ATURAT) {
			this->rot -= (float)delta * 25;
		}
		break;
	case 3:
		if (rotCounter == 0) {
			canviEstat = true;
		}
		break;
	default:
		break;
	}
	this->rot = fmod(this->rot, 360.0f);

	Vector3<float> front = Vector3<float>(-sinf(toRad(this->rot)), 0, -cosf(toRad(this->rot)));
	Vector3<float> dir = Vector3<float>::normalize(front);
	if (this->estat == Estat::CAMINANT) {
		this->speed.x = dir.x;
		this->speed.z = dir.z;
	}
	Vector3<float> newPos = this->pos + dir;

	Bloc nbd = world->getBlock(newPos - Vector3<float>(0, 1.0f, 0));
	Bloc nba = world->getBlock(newPos + Vector3<float>(0, 0, 0));
	Bloc nbu = world->getBlock(newPos + Vector3<float>(0, 1, 0));
	//this->pos = this->pos; /*Vector3<float>(sinf(toRad(this->rot)), 0, sinf(toRad(this->rot)))/100.0f*/;
	//this->pos.x += 0.001f;;

	//printf("abaix %d normal %d amunt %d grav %f\n", nbd, nba, nbu, this->grav);
	if (!Block::isSolid(nba) && Block::isSolid(nbd) && this->estat == Estat::CAMINANT) {
		this->grav = -4.0f;
	}

	if (canviEstat) {
		if (nba == Bloc::HERBA && this->estat != Estat::MENJANT) {
			this->estat = Estat::MENJANT;
			//printf("EAT BEGIN\n");
		}
		else {
			int r = rand() % 3;
			Estat antest = this->estat;
			if (r == 2) {
				this->estat = Estat::ATURAT;
				this->speed = Vector3<float>(0, 0, 0);
			}
			else {
				this->estat = Estat::CAMINANT;
			}
			if (antest == Estat::MENJANT) {
				//printf("EAT END\n");
				//world->setBlock(Bloc::RES, newPos.toInt(), true, true); //No lleva el que toca, sinó el de darrera
			}
		}
	}
	if (this->grav < 0) { //Si tocam adalt, tornam caure
		if (Block::isSolid(nbu)) {
			this->grav = 0;
		}
	}
}


//Funció de dibuix
void Ostrich::draw(double delta) {
	glScalef(1.75f, 1.75f, 1.75f);

	int est = static_cast<int>(this->estat);
	int frameMax = frameInici[est] + frameCount[est];
	int frameAct = (int)(anim+=((float)delta/30.0f)) + frameInici[est];

	TextureManager::applyTexture("Estruç");
	glRotatef(rot, 0, 1, 0);

	if (frameAct >= frameMax) {
		frameAct = 0;
		anim = 0;
	}

	ModelManager::drawModel("Estruç", frameAct);

	TextureManager::noTexture();

}

void Ostrich::destroy() {
}

void Ostrich::setCam(Camera* cam) {
	cam->setPos(this->pos + Vector3<float>(0, eyesOffset, 0));
}
