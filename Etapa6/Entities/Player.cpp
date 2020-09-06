#include "Player.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

#include "../Material.h"

class World;

Player::Player(World* world, Vector3 pos): Entity(world, pos)
{

}

/**
  * Funció d'actualització de l'estat intern del jugador (passiva)
  */
void Player::update(int delta) {
	if (this->gamemode == 1) {
		Vector3 grav = Vector3(0, -1, 0) * (((float)delta / 100.0f) * this->grav);
		Vector3 newPos = this->pos + grav;
		if (!Block::isSolid(world->getBlock(newPos - Vector3(0, 1, 0)))) {
			this->pos = newPos;
		}
		if (this->grav < 1) {
			this->grav += 0.001f * delta;
		}
	}
}

//Control per teclat
void Player::control(unsigned char key) {
	if (key == ' ' && Block::isSolid(world->getBlock(this->pos - Vector3(0, 2, 0))) && this->gamemode == 1) {
		grav = -1;
	}
}

/*
  Funció d'actualització de l'estat intern del cotxe (activa, quan està controlat)
*/
void Player::control(int delta, Camera *cam) {
	//Actualitzam la posició del jugador
	Vector3 add = Vector3(0, 0, 0);
	if (KeyboardManager::isPressed('w')) {
		add = add + Vector3(cam->getFront().x, 0, cam->getFront().z);
	}
	else if (KeyboardManager::isPressed('s')) {
		add = add - Vector3(cam->getFront().x, 0, cam->getFront().z);
	}
	if (KeyboardManager::isPressed('d')) {
		add = add - cam->getRight();
	}
	else if (KeyboardManager::isPressed('a')) {
		add = add + cam->getRight();
	}
	if (KeyboardManager::isPressed(' ')) {
		add = add + Vector3(0, 1, 0);
	}
	else if (KeyboardManager::isPressed('{')) {
		add = add - Vector3(0, 1, 0);
	}
	Vector3 toAdd = add * ((float)delta / 100.0f);
	//printf("%f, %f, %f\n", toAdd.x, toAdd.y, toAdd.z);
	Vector3 newPos = this->pos + add * ((float)delta / 100.0f);
	printf("%f %f %f. %f, %f, %f. %f, %f, %f\n", toAdd.x, toAdd.y, toAdd.z, this->pos.x, this->pos.y, this->pos.z, newPos.x, newPos.y, newPos.z);
	if (Block::isSolid(world->getBlock(newPos - Vector3(0,1,0)))) {
		Vector3 poss[6] = { Vector3(add.x, add.y, 0), Vector3(add.x, 0, add.z), Vector3(0, add.y, add.z), 
			Vector3(add.x, 0, 0), Vector3(0, add.y, 0), Vector3(0, 0, add.z) };
		for (int i = 0; i < 6; i++) {
			Vector3 newAdd = poss[i];
			newPos = this->pos + newAdd * ((float)delta / 100.0f);
			if (!Block::isSolid(world->getBlock(newPos - Vector3(0,1,0)))) {
				this->pos = newPos;
				break;
			}
		}
	}
	else {
		this->pos = newPos;
	}

}

//Funció de dibuix
void Player::draw() {
	//Model jugador
}

void Player::destroy() {
}

void Player::setCam(Camera* cam) {
	cam->setPos(this->pos - cam->getFront()*1.0f);
}

void Player::onAttach() {

}

void Player::onDeattach(){

}