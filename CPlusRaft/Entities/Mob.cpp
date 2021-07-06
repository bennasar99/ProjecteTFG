#include "Mob.h"

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

class World;

Mob::Mob(World* world, Vector3<float> pos, float height): Entity(world, pos, height)
{
	this->health = 5;
	this->rot = float(rand() % 300);
}

/**
  * Funció d'actualització de l'estat intern del mob
  */
void Mob::update(double delta) {
	Entity::update(delta);

	if (this->world->getBlock(this->pos) == Bloc::AIGUA) { //Push
		this->grav -= (float)delta * GRAVITY;
	}

	float offset = this->height / 2.0f;
	rotCounter += (float)delta;
	if (rotCounter > 5) {
		rotAct = rand() % 5;
		rotCounter = 0;
	}

	
	this->rot = fmod(this->rot, 360.0f);
	Vector3<float> front = Vector3<float>(-sinf(toRad(this->rot)), 0, -cosf(toRad(this->rot)));
	Vector3<float> dir = Vector3<float>::normalize(front);
	this->speed.x = dir.x;
	this->speed.z = dir.z;

	Vector3<float> newPos = this->pos + dir;
	Bloc nbd = world->getBlock(newPos - Vector3<float>(0, offset, 0));
	Bloc nba = world->getBlock(newPos + Vector3<float>(0, 0, 0));
	Bloc nbu = world->getBlock(newPos + Vector3<float>(0, offset, 0));

	//printf("abaix %d normal %d amunt %d grav %f\n", nbd, nba, nbu, this->grav);
	if (!Block::isSolid(nba) && Block::isSolid(nbd)) {
		this->grav = -5.0f;
	}
}

void Mob::destroy() {
}

void Mob::setCam(Camera* cam) {
	cam->setPos(this->pos + Vector3<float>(0, eyesOffset, 0));
}
