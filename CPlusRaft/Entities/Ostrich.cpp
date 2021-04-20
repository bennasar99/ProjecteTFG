#include "Ostrich.h"

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

class World;

Ostrich::Ostrich(World* world, Vector3<float> pos): Entity(world, pos)
{
	this->health = 5;
	this->estat = Estat::CAMINANT;
}

/**
  * Funció d'actualització de l'estat intern del jugador (passiva)
  */
void Ostrich::update(double delta) { 
	rotCounter += delta;
	//printf("rc %f\n", rotCounter);
	if (rotCounter > 5) {
		rotAct = rand() % 5;
		printf("ra %d", rotAct);
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

	//this->pos = this->pos; /*Vector3<float>(sinf(toRad(this->rot)), 0, sinf(toRad(this->rot)))/100.0f*/;
	//this->pos.x += 0.001f;;

	//Gravetat
	Bloc bd = world->getBlock(this->pos - Vector3<float>(0, 1, 0));
	Bloc ba = world->getBlock(this->pos + Vector3<float>(0, 0, 0));
	Bloc bu = world->getBlock(this->pos + Vector3<float>(0, 1, 0));

	if (this->grav >= 0 && Block::isSolid(bd)) {
		this->grav = 0;
	}
	else if (this->grav < gravmax) { //"Gravetat"
		float mult = 1;
		if (ba == Bloc::AIGUA) { //A l'aigua queim més lent
			mult = -1.0f;
		}
		this->grav += (float)delta * 9.8f * mult;

		if (ba == Bloc::AIGUA) {
			this->grav = std::max(this->grav, -1.0f);
		}
	}
	Vector3<float> newPos = this->pos + Vector3<float>(dir.x, 0, dir.z) * 0.03f + Vector3<float>(0, -1, 0) * (float)delta * this->grav;
	Vector3<float> checkPos = newPos;
	Bloc nbd = world->getBlock(newPos - Vector3<float>(0, 1.0f, 0));
	Bloc nba = world->getBlock(newPos + Vector3<float>(0, 0, 0));
	Bloc nbu = world->getBlock(newPos + Vector3<float>(0, 1, 0));
	//printf("abaix %d normal %d amunt %d grav %f\n", nbd, nba, nbu, this->grav);
	if (nbu == Bloc::RES && Block::isSolid(nba)) {
		this->grav = -5.0f;
		newPos = this->pos + Vector3<float>(dir.x, 0, dir.z) * 0.1f + Vector3<float>(0, 1, 0);
	}

	if (canviEstat) {
		if (nba == Bloc::HERBA && this->estat != Estat::MENJANT) {
			this->estat = Estat::MENJANT;
			printf("EAT BEGIN\n");
		}
		else {
			int r = rand() % 3;
			Estat antest = this->estat;
			if (r == 2) {
				this->estat = Estat::ATURAT;
			}
			else {
				this->estat = Estat::CAMINANT;
			}
			if (antest == Estat::MENJANT) {
				printf("EAT END\n");
				world->setBlock(Bloc::RES, newPos.toInt(), true, true); //No lleva el que toca, sinó el de darrera
			}
		}
	}

	if (!Block::isSolid(world->getBlock(newPos)) && this->estat == Estat::CAMINANT) { //Caiem
		this->pos = newPos;
	}
	if (this->grav < 0) { //Si tocam adalt, tornam caure
		if (Block::isSolid(bu)) {
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

	glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::ESTRUC));
	glRotatef(rot, 0, 1, 0);

	ModelManager::drawModel(Model::ESTRUC, frameAct);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (frameAct >= frameMax) {
		anim = 0;
	}

}

void Ostrich::destroy() {
}

void Ostrich::setCam(Camera* cam) {
	cam->setPos(this->pos + Vector3<float>(0, eyesOffset, 0));
}
