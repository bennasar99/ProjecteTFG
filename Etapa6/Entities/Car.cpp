#include "Car.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

#include "../Material.h"

class World;

Car::Car(World* world, Vector3 pos, int model): Entity(world, pos)
{
	this->model = model;

	//Preparam els llums del cotxe
	this->llumdret = world->addLight(pos);
	this->llumesq = world->addLight(pos);
	this->llumdret->setEnabled(false);
	this->llumesq->setEnabled(false);

	this->llumesq->setSpreadAngle(90.0f);
	this->llumdret->setSpreadAngle(90.0f);
	this->llumdret->setLightConcentration(0);
	this->llumesq->setLightConcentration(0);

	this->llumdret->setAttenuation(Attenuation::QUADRATIC, 0.1f);
	this->llumesq->setAttenuation(Attenuation::QUADRATIC, 0.1f);
	this->llumdret->setAttenuation(Attenuation::LINEAR, 0);
	this->llumesq->setAttenuation(Attenuation::LINEAR, 0);
	this->llumdret->setAttenuation(Attenuation::CONSTANT, 0.0f);
	this->llumesq->setAttenuation(Attenuation::CONSTANT, 0.0f);

	// carregar l'objecte
	if (this->model == 0) {
		this->carMesh.load("Models/car2.obj");
	}

	this->controllable = true; //El cotxe és controlable

}

void Car::updateLights() { //Actualitzam la direcció de les llums

	llumesq->setDir(Vector3(1 * sinf(toRad(rot)), 0, 1 * cosf(toRad(rot))));
	llumdret->setDir(Vector3(1 * sinf(toRad(rot)), 0, 1 * cosf(toRad(rot))));
}

/**
  * Funció d'actualització de l'estat intern del cotxe (passiva)
  */
void Car::update(int delta) {

}

//Control per teclat
void Car::control(unsigned char key) {
	if (key == 'f' || key == 'F') {
		this->lightsOn(!this->getLightsOn());
	}
}

/*
  Funció d'actualització de l'estat intern del cotxe (activa, quan està controlat)
*/
void Car::control(int delta) {
	//Actualitzam l'acceleració, segons la tecla pitjada
	if (KeyboardManager::isPressed('w')) {
		SoundManager::playSound(So::ACCELERA, this->pos, false);
		acc += 0.001f * delta;
	}
	if (KeyboardManager::isPressed('s')) {
		acc -= 0.001f * delta;
	}
	if (KeyboardManager::isPressed('d')) {
		rot -= 0.1f * delta;
	}
	if (KeyboardManager::isPressed('a')) {
		rot += 0.1f * delta;
	}
	acc = std::min(acc, 0.5f);
	acc = std::max(acc, -0.2f);

	//Actualitzam la velocitat
	this->vel = this->vel + Vector3(this->acc * sinf(toRad(rot)), 0, this->acc * cosf(toRad(rot)));
	this->vel.x = std::min(this->vel.x, 0.05f);
	this->vel.x = std::max(this->vel.x, -0.05f);
	this->vel.z = std::min(this->vel.z, 0.05f);
	this->vel.z = std::max(this->vel.z, -0.05f);

	//Actualitzam la posició
	this->pos = this->pos + this->vel*((float)delta/10.0f);

}

//Funció de dibuix
void Car::draw() {
	float rotRodes = 0; //Rotació de les rodes pel model 1
	if (this->enMarxa) { //Si l'usuari està controlant el cotxe, el dibuixam rotat cap on vagi
		if (KeyboardManager::isPressed('d')) {
			glRotatef(-15, 0, 1, 0);
			rotRodes = -30;
		}
		else if (KeyboardManager::isPressed('a')) {
			glRotatef(+15, 0, 1, 0);
			rotRodes = 30;
		}
	}
	glRotatef(rot, 0, 1, 0);

	//Actualitzam la posició dels llums
	glPushMatrix();
	this->updateLights();
	glLoadIdentity();
	Vector3 right = Vector3::normalize(Vector3::cross(world->camera->getFront(), Vector3(0, 1, 0))) * 0.5f;
	llumesq->setPos(this->getPos() + Vector3(1.5f * sinf(toRad(rot)), 0.5f, 1.5f * cosf(toRad(rot))) - right);
	llumdret->setPos(this->getPos() + Vector3(1.5f * sinf(toRad(rot)), 0.5f, 1.5f * cosf(toRad(rot))) + right);
	glPopMatrix();

	//Dibuixam el cotxe, segons el model
	switch (this->model) {
	case 0:
		carMesh.draw();
		break;
	case 1:
		drawCar(rotRodes);
		break;
	}
}

void Car::destroy() {
	//Eliminam els llums del món
    world->delLight(this->llumesq);
	world->delLight(this->llumdret);

	this->llumdret = 0;
	this->llumesq = 0;
}

//Llums cotxe ON/OFF
void Car::lightsOn(bool lights) {
	this->llumdret->setEnabled(lights);
	this->llumesq->setEnabled(lights);
}

bool Car::getLightsOn() {
	return (this->llumdret->getEnabled() && this->llumesq->getEnabled());
}

void Car::setCam(Camera* cam) {
	cam->setPos(this->getPos() + Vector3(-6 * sinf(toRad(this->getRot())), 3, -6 * cosf(toRad(this->getRot()))));
	cam->lookAt(this->getPos());
}

void Car::onAttach() {
	this->enMarxa = true;
}

void Car::onDeattach(){
	this->enMarxa = false;
}