#include "Grua.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

#include "../Material.h"

class World;

Grua::Grua(World* world, Vector3 pos): Entity(world, pos)
{
	this->pos = this->pos; //Hack càmera, perquè quedi centrada

	// carregar l'objecte
	this->base.load("Models/Grua/base.obj");
	this->carrier.load("Models/Grua/carrier.obj");
	this->crane1.load("Models/Grua/crane1.obj");
	this->crane2.load("Models/Grua/crane2.obj");
	this->hook.load("Models/Grua/hook.obj");

	this->controllable = true; //La grua es pot controlar
}

/**
  * Funció d'actualització de l'estat intern de la grua (passiva)
  */
void Grua::update(int delta) {

}

//Control per teclat
void Grua::control(unsigned char key) {
	if (key == 'q' || key == 'Q') { //Agafam/mollam
		if (this->ent != 0) {
			this->ent = 0;
		}
		else if (this->bloc != 0) {
			if (world->getBlock(hookPos) == Bloc::RES) {
				world->setBlock(this->bloc, hookPos, false); //Potser true
			}
			this->bloc = 0;
		}
		else {
			this->ent = world->getNearestEntity(hookPos, 4.0f, false);
			if (this->ent == 0) { //Si no hi ha una entitat propera, agafam un bloc (si n'hi ha)
				this->bloc = world->getBlockPointer(hookPos, true);
			}
		}
	}
}

//Funció d'actualització activa (quan l'entitat està controlada)
void Grua::control(int delta) {
	//Actualitzam la rotació
	if (KeyboardManager::isPressed('d')) {
		rot -= 0.1f * delta;
	}
	else if (KeyboardManager::isPressed('a')) {
		rot += 0.1f * delta;
	}

	//Actualitzam la posició del ganxo
	if (KeyboardManager::isPressed('w')) {
		if (hookX > 0) {
			hookX -= 0.1f * delta;
		}
	}
	else if (KeyboardManager::isPressed('s')) {
		if (hookX < 160) {
			hookX += 0.1f * delta;
		}
	}
	if (KeyboardManager::isPressed('z')) {
		if (hookY > -140) {
			hookY -= 0.1f * delta;
		}
	}
	else if (KeyboardManager::isPressed('x')) {
		if (hookY < 0) {
			hookY += 0.1f * delta;
		}
	}
	hookPos = this->pos + Vector3((10 - hookX / 20) * sinf(toRad(rot - 45)), hookY / 20.0f + 7.0f, (10 - hookX / 20) * cosf(toRad(rot - 45)));

	if (this->ent != 0) { //Actualitzam la posició de l'entitat agafada
		this->ent->setPos(hookPos);
	}
}

//Funció de dibuix
void Grua::draw() {
	glPushMatrix();
	
	if (this->bloc != 0) { //Si la grua té un bloc agafat, el dibuixam
		glPushMatrix();
		Vector3 pos = Vector3((10 - hookX / 20) * sinf(toRad(rot - 45)), hookY / 20.0f + 7, (10 - hookX / 20) * cosf(toRad(rot - 45)));
		glTranslatef(pos.x, pos.y, pos.z);
		//world->br->drawBloc(bloc->getId());
		glPopMatrix();
	}

	//Dibuixam les diferents parts de la grua
	glScalef(0.05f, 0.05f, 0.05f);
	this->base.draw();
	this->crane1.draw();

	//Rotació de la part superior de la grua
	glTranslatef(3, 0, 3);
	glRotatef(45, 0, 1, 0);
	glRotatef(rot, 0, 1, 0);
	this->crane2.draw();
	glTranslatef(hookX, 0, 0);
	this->carrier.draw();
	glTranslatef(0, hookY, 0);
	this->hook.draw();

	glPopMatrix();

}

void Grua::destroy() {
	//
}

void Grua::setCam(Camera* cam) {
	cam->setPos(this->pos + Vector3(-6 * sinf(toRad(this->getRot())), 13, -6 * cosf(toRad(this->getRot()))));
	cam->lookAt(this->pos + Vector3(0,8,0));
}