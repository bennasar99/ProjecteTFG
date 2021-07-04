#include "Entity.h"
#include "../World.h"
#include <iostream>

Entity::Entity(World* world, Vector3<float> pos, float height){
	this->world = world;
	this->pos = pos;
	this->height = height;
}

Entity::Entity() {
	this->world = NULL;
	this->pos = Vector3<float>(0, 0, 0);
}

void Entity::draw(double delta) {}

void Entity::update(double delta) {

	//Gravetat
	if (!checkCollisions) {
		//printf("pos %f %f %f speed %f %f %f delta %f\n", this->pos.x, this->pos.y, this->pos.z, this->speed.x, this->speed.y, this->speed.z, (float)delta);
		this->pos = this->pos + this->speed * (float)delta;
		return;
	}
	float offset = this->height / 2.0f;
	Bloc bd = world->getBlock(this->pos - Vector3<float>(0, offset, 0));
	Bloc ba = world->getBlock(this->pos + Vector3<float>(0, 0, 0));
	Bloc bu = world->getBlock(this->pos + Vector3<float>(0, offset, 0));

	if (this->grav >= 0 && Block::isSolid(bd)) {
		this->grav = 0;
	}
	else if (this->grav < GRAVMAX) { //"Gravetat"
		float mult = 1.0f;
		if (ba == Bloc::AIGUA) { //A l'aigua queim més lent
			mult = -1.0f;
		}
		this->grav += (float)delta * GRAVITY * mult;
		if (ba == Bloc::AIGUA) {
			this->grav = std::max(this->grav, -1.0f);
		}
	}

	//printf("abaix %d normal %d amunt %d grav %f\n", nbd, nba, nbu, this->grav);
	/*if (nbu == Bloc::RES && Block::isSolid(nba)) {
		this->grav = -5.0f;
		newPos = this->pos + Vector3<float>(0, 1, 0);
	}*/
	this->speed.y = -1.0f * this->grav;
	printf("speedY %f\n", this->speed.y);
	Vector3<float> add = this->speed * (float)delta;
	Vector3<float> newPos = this->pos + add;
	Vector3<float> checkPos = newPos;
	Bloc nbd = world->getBlock(newPos - Vector3<float>(0, offset, 0));
	Bloc nba = world->getBlock(newPos);
	Bloc nbu = world->getBlock(newPos + Vector3<float>(0, offset, 0));

	if (newPos != this->pos) {
		if (Block::isSolid(world->getBlock(newPos - Vector3<float>(0, 1, 0))) ||
			Block::isSolid(world->getBlock(newPos))) {
			Vector3<float> poss[6] = { Vector3<float>(add.x, 0, add.z), Vector3<float>(add.x, add.y, 0), Vector3<float>(0, add.y, add.z),
				Vector3<float>(add.x, 0, 0), Vector3<float>(0, add.y, 0), Vector3<float>(0, 0, add.z) };
			for (int i = 0; i < 6; i++) {
				Vector3<float> newAdd = poss[i];
				newPos = this->pos + newAdd;
				if (!Block::isSolid(world->getBlock(newPos - Vector3<float>(0, 1, 0))) &&
					!Block::isSolid(world->getBlock(newPos + Vector3<float>(0, 0, 0)))) {
					this->pos = newPos;
					break;
				}
			}
		}
		else {
			this->pos = newPos;
		}
	}

	if (!Block::isSolid(world->getBlock(newPos)) && this->grav >= 0.0f) { //Caiem
		this->pos = newPos;
	}
}

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