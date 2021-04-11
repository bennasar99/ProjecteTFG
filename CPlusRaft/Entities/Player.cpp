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
void Player::update(float delta) {
	if (this->gamemode == 1) { //Gravetat MILLORAR
		Bloc bd = world->getBlock(this->pos - Vector3<float>(0, 1.5f, 0));
		Bloc ba = world->getBlock(this->pos - Vector3<float>(0, 0.5f, 0));
		Bloc bu = world->getBlock(this->pos + Vector3<float>(0, eyesOffset + 0.2f, 0));

		if (this->grav >= 0 && Block::isSolid(bd)) {
			this->grav = 0;
			return;
		}
		else if (this->grav < gravmax) { //"Gravetat"
			float mult = 1;
			if (ba == Bloc::AIGUA) { //A l'aigua queim més lent
				if (this->grav > 2.0f) { //Reduim la gravetat, (push de l'aigua cap a dalt en tirar-s'hi)
					mult = -1.0f;
				}
				else {
					mult = 0.5f; //Gravetat a la meitat
				}
				if (KeyboardManager::isPressed(GLFW_KEY_SPACE)) {
					mult += -2.0f;
				}
			}
			this->grav += delta * 9.8f * mult;

			if (ba == Bloc::AIGUA) {
				this->grav = std::max(this->grav, -0.2f);
			}
		}
		Vector3<float> newPos = this->pos + Vector3<float>(0, -1, 0) * delta * this->grav;
		if (!Block::isSolid(world->getBlock(newPos)) && this->grav >= 0.0f) { //Caiem
			this->pos = newPos;
		}
		if (this->grav < 0) { //Si tocam adalt, tornam caure
			if (Block::isSolid(bu)) {
				this->grav = 0;
			}
		}
	}
}

//Control per teclat
void Player::control(int key) {
	if (key == GLFW_KEY_SPACE && Block::isSolid(world->getBlock(this->pos - Vector3<float>(0, 2, 0))) && grav >= 0.00f && this->gamemode == 1) {
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
  Funció d'actualització de l'estat intern del jugador (activa, quan està controlat)
*/
void Player::control(float delta, Camera *cam) {
	//Actualitzam la posició del jugador
	Vector3<float> add = Vector3<float>(0, 0, 0);
	Vector3<float> forward = Vector3<float>::normalize(Vector3<float>(cam->getFront().x, 0, cam->getFront().z));
	Vector3<float> right = Vector3<float>::normalize(Vector3<float>(cam->getRight().x, 0, cam->getRight().z));
	Vector3<float> up = Vector3<float>(0, 1, 0);
	if (KeyboardManager::isPressed(GLFW_KEY_W)) {
		add = add + forward;
	}
	else if (KeyboardManager::isPressed(GLFW_KEY_S)) {
		add = add - forward;
	}
	if (KeyboardManager::isPressed(GLFW_KEY_D)) {
		add = add - right;
	}
	else if (KeyboardManager::isPressed(GLFW_KEY_A)) {
		add = add + right;
	}
	if ((KeyboardManager::isPressed(GLFW_KEY_SPACE) && this->gamemode==0) || (grav < 0)) { //Creatiu o acabam de botar
		add = add + up;
	}
	else if (KeyboardManager::isPressed(GLFW_KEY_LEFT_CONTROL) && this->gamemode == 0) {
		add = add - up;
	}
	if (KeyboardManager::isPressed(GLFW_KEY_LEFT_CONTROL)) { //Crouch
		this->eyesOffset = 0.0f;
	}
	else {
		this->eyesOffset = 0.5f;
	}
	float speed = this->speed;
	if (KeyboardManager::isPressed(GLFW_KEY_LEFT_SHIFT)) { //Si sprinta
		speed*=2.0f;
	}
	Vector3<float> newPos = this->pos + add * delta * speed;
	if (gamemode == 0) {
		this->pos = newPos;
		return; //No comprovam colisions
	}
	if (newPos != this->pos) {
		if (Block::isSolid(world->getBlock(newPos + add * ((float)delta) - Vector3<float>(0, 1, 0))) ||
			Block::isSolid(world->getBlock(newPos + add * ((float)delta)))) {
			Vector3<float> poss[6] = { Vector3<float>(add.x, add.y, 0), Vector3<float>(add.x, 0, add.z), Vector3<float>(0, add.y, add.z),
				Vector3<float>(add.x, 0, 0), Vector3<float>(0, add.y, 0), Vector3<float>(0, 0, add.z) };
			for (int i = 0; i < 6; i++) {
				Vector3<float> newAdd = poss[i];
				newPos = this->pos + newAdd * ((float)delta / 200.0f);
				if (!Block::isSolid(world->getBlock(newPos + newAdd * ((float)delta) - Vector3<float>(0, 1, 0))) &&
					!Block::isSolid(world->getBlock(newPos + newAdd * ((float)delta) + Vector3<float>(0, eyesOffset, 0)))) {
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