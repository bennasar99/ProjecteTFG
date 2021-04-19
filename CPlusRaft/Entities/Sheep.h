#pragma once
#include "../Utils/Vector3.h"
#include "Entity.h"
#include <list>
#include "../Utils/KeyboardManager.h"
#include "../Utils/SoundManager.h"
#include "../Light.h"
#include "../Render/Mesh.h"

class Block;
class World;

/*
	Entitat jugador
*/
class Sheep : public Entity {
private:
	float grav = 1;
	float gravmax = 100;
	float speed = 16.0f;

	int health;
	float rotCounter = 0;
	float rot = 0;
	float rotDavDr = 0;
	float rotDR = 1;
	short rotAct = 0;

	float eyesOffset = 0.5f; //Alçada ulls

public:
	Sheep(World* world, Vector3<float> pos);

	void update(float delta);

	void draw();
	void destroy();

	void setCam(Camera* cam);
};

