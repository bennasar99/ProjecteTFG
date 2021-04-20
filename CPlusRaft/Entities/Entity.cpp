#include "Entity.h"
#include "../World.h"
#include <iostream>

Entity::Entity(World* world, Vector3<float> pos) {
	this->world = world;
	this->pos = pos;
}

Entity::Entity() {
	this->world = NULL;
	this->pos = Vector3<float>(0, 0, 0);
}

void Entity::draw(double delta) {}

void Entity::update(double delta) {}

void Entity::destroy() {}

Vector3<float> Entity::getPos() {
	return this->pos;
}

void Entity::setPos(Vector3<float> pos) {
	this->pos = pos;
}

void Entity::setRot(float rot) {
	this->rot = fmodf(rot, 360.0f);
	if (this->rot >= 360) {
		this->rot -= 360;
	}
	if (this->rot < 0.0f) {
		this->rot += 360;
	}
}

float Entity::getRot() {
	return this->rot;
}

void Entity::control(int key) {}

void Entity::control(double delta) {}

void Entity::control(double delta, Camera* cam) {}

void Entity::setCam(Camera* cam) {}

bool Entity::getControllable() {
	return this->controllable;
}

void Entity::onAttach() {

}

void Entity::onDeattach() {

}