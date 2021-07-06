#include "Ostrich.h"

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

class World;

Ostrich::Ostrich(World* world, Vector3<float> pos): Mob(world, pos, 2.2f)
{
	this->health = 5;
	this->estat = Estat::CAMINANT;
}

/**
  * Funció d'actualització de l'estat intern de l'estruç
  */
void Ostrich::update(double delta) {
	Mob::update(delta);

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
	}

	canviEstatCounter += (float)delta;
	if (canviEstatCounter > 5) {
		canviEstat = true;
		canviEstatCounter = 0.0f;
	}
	
	if (canviEstat) {
		if (world->getBlock(this->pos) == Bloc::HERBA && this->estat != Estat::MENJANT) {
			this->speedM = 0;
			this->estat = Estat::MENJANT;
		}
		else {
			int r = rand() % 3;
			Estat antest = this->estat;
			if (r == 2) {
				this->estat = Estat::ATURAT;
				this->speedM = 0;
			}
			else {
				this->estat = Estat::CAMINANT;
				this->speedM = 1.0f;
			}
			if (antest == Estat::MENJANT) {
				//world->setBlock(Bloc::RES, newPos.toInt(), true, true); //No lleva el que toca, sinó el de darrera
			}
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
