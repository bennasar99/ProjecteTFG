#include "Player.h"

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

class World;

Player::Player(World* world, Vector3<float> pos): Entity(world, pos)
{
	this->health = 5;
}

/**
  * Funció d'actualització de l'estat intern del jugador (passiva)
  */
void Player::update(int delta) {
	if (this->gamemode == 1) { //Gravetat
		Vector3<float> grav = Vector3<float>(0, -1, 0) * (((float)delta / 100.0f) * this->grav);
		if (world->getBlock(this->pos) == Bloc::AIGUA) { //A l'aigua queim més lent
			grav = grav / 4.0f;
		}
		Vector3<float> newPos = this->pos + grav;
		if (!Block::isSolid(world->getBlock(newPos - Vector3<float>(0, 1, 0))) && this->grav >= 0) { //Caiem
			this->pos = newPos;
		}
		if (this->grav < 1) { //"Gravetat"
			this->grav += 0.003f * delta;
		}
		if (this->grav < 0) { //Si tocam adalt, tornam caure
			if (Block::isSolid(world->getBlock(this->pos + Vector3<float>(0,eyesOffset + 0.2f,0)))) {
				this->grav = 0;
			}
		}

		if (KeyboardManager::isPressed(GLFW_KEY_SPACE) && world->getBlock(this->pos) == Bloc::AIGUA && this->gamemode == 1) {
			this->grav = -0.3f;
		}
	}
}

//Control per teclat
void Player::control(int key) {
	if (key == GLFW_KEY_SPACE && Block::isSolid(world->getBlock(this->pos - Vector3<float>(0, 2, 0))) && grav >= 1 && this->gamemode == 1) {
		grav = -1.0f;
	}
	if (key == GLFW_KEY_G) {
		if (gamemode == 0) {
			gamemode = 1;
		}
		else {
			gamemode = 0;
		}
	}
}

/*
  Funció d'actualització de l'estat intern del cotxe (activa, quan està controlat)
*/
void Player::control(int delta, Camera *cam) {
	//Actualitzam la posició del jugador
	Vector3<float> add = Vector3<float>(0, 0, 0);
	Vector3<float> forward = Vector3<float>::normalize(Vector3<float>(cam->getFront().x, 0, cam->getFront().z)) * this->speed;
	if (KeyboardManager::isPressed(GLFW_KEY_W)) {
		add = add + forward;
	}
	else if (KeyboardManager::isPressed(GLFW_KEY_S)) {
		add = add - forward;
	}
	if (KeyboardManager::isPressed(GLFW_KEY_D)) {
		add = add - cam->getRight();
	}
	else if (KeyboardManager::isPressed(GLFW_KEY_A)) {
		add = add + cam->getRight();
	}
	if ((KeyboardManager::isPressed(GLFW_KEY_SPACE) && this->gamemode==0) || (grav < 0)) { //Creatiu o acabam de botar
		add = add + Vector3<float>(0, 1, 0);
	}
	else if (KeyboardManager::isPressed('{') && this->gamemode == 0) {
		add = add - Vector3<float>(0, 1, 0);
	}
	if (KeyboardManager::isPressed('}')) { //Crouch
		this->eyesOffset = 0.0f;
	}
	else {
		this->eyesOffset = 0.5f;
	}
	if (KeyboardManager::isPressed('{') && this->gamemode == 1) {
		this->speed = 2;
	}
	else if (this->gamemode == 1) {
		this->speed = 1;
	}
	Vector3<float> newPos = this->pos + add * ((float)delta / 200.0f);
	if (newPos != this->pos) {
		if (Block::isSolid(world->getBlock(newPos + add * ((float)delta / 200.0f) - Vector3<float>(0, 1, 0))) ||
			Block::isSolid(world->getBlock(newPos + add * ((float)delta / 200.0f)))) {
			Vector3<float> poss[6] = { Vector3<float>(add.x, add.y, 0), Vector3<float>(add.x, 0, add.z), Vector3<float>(0, add.y, add.z),
				Vector3<float>(add.x, 0, 0), Vector3<float>(0, add.y, 0), Vector3<float>(0, 0, add.z) };
			for (int i = 0; i < 6; i++) {
				Vector3<float> newAdd = poss[i];
				newPos = this->pos + newAdd * ((float)delta / 200.0f);
				if (!Block::isSolid(world->getBlock(newPos + newAdd * ((float)delta / 200.0f) - Vector3<float>(0, 1, 0))) &&
					!Block::isSolid(world->getBlock(newPos + newAdd * ((float)delta / 200.0f) + Vector3<float>(0, eyesOffset, 0)))) {
					this->pos = newPos;
					if (Block::isSolid(world->getBlock(this->pos - Vector3<float>(0,1,0)))) {
						SoundManager::playSound(So::CAMINA, this->pos, false);
					}
					break;
				}
			}
		}
		else {
			this->pos = newPos;
			if (world->getBlock(this->pos - Vector3<float>(0, 1, 0)) != Bloc::AIGUA) {
				SoundManager::playSound(So::CAMINA, this->pos, false);
			}
		}
	}

}

//Funció de dibuix
void Player::draw() {
	//Model jugador
}

void Player::destroy() {
}

void Player::setCam(Camera* cam) {
	cam->setPos(this->pos + Vector3<float>(0, eyesOffset, 0));
}

void Player::onAttach() {

}

void Player::onDeattach(){

}