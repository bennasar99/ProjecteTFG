#include "Player.h"

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

class World;

Player::Player(World* world, Vector3<float> pos): Entity(world, pos, 3.0f)
{
	this->health = 5;
	this->speedM = 5;
	this->checkCollisions = false;
}

/**
  * Funció d'actualització de l'estat intern del jugador (passiva)
  */
void Player::update(double delta) {
	Entity::update(delta);
}

//Control per teclat
void Player::control(int key) {
	if (key == GLFW_KEY_SPACE){
		if (Block::isSolid(world->getBlock(this->pos - Vector3<float>(0, 2, 0))) && grav >= 0.00f && this->gamemode == 1) {
			grav = -5.0f;
		}
		
	}
	if (key == GLFW_KEY_G) {
		if (gamemode == 0) {
			gamemode = 1;
			this->speed.y = 0; //Per si està botant en aquell moment no volar després
			this->checkCollisions = true;
		}
		else {
			this->checkCollisions = false;
			gamemode = 0;
		}
	}
}

/*
  Funció d'actualització de l'estat intern del jugador (activa, quan està controlat)
*/
void Player::control(double delta, Camera *cam) {
	this->speed = Vector3<float>(0, 0, 0);

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
	if (KeyboardManager::isPressed(GLFW_KEY_SPACE) && (this->gamemode==0 || world->getBlock(this->pos) == Bloc::AIGUA)) { //Creatiu o dins l'aigua
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
	if (add.x == 0 && add.y == 0 && add.z == 0) {
		return;
	}
	float speedM = this->speedM;
	if (KeyboardManager::isPressed(GLFW_KEY_LEFT_SHIFT)) { //Si sprinta
		speedM*=2.0f;
	}
	if (Block::isSolid(world->getBlock(this->pos - Vector3<float>(0, 1.5f, 0)))) {
		SoundManager::playSound("Camina", this->pos, false);
	}
	this->speed = Vector3<float>::normalize(add) * speedM;
	//printf("speedP %f %f %f\n", speed.x, speed.y, speed.z);
}

//Funció de dibuix
void Player::draw(double delta) {
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