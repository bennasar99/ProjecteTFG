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

void Entity::draw() {}

void Entity::update(int delta) {}

void Entity::destroy() {}

Vector3<float> Entity::getPos() {
	return this->pos;
}

void Entity::setPos(Vector3<float> pos) {
	this->pos = pos;
}

float Entity::getRot() {
	return this->rot;
}

void Entity::control(unsigned char key) {}

void Entity::control(int delta) {}

void Entity::control(int delta, Camera* cam)
{
}

void Entity::setCam(Camera* cam) {}

bool Entity::getControllable() {
	return this->controllable;
}

void Entity::onAttach() {

}

void Entity::onDeattach() {

}