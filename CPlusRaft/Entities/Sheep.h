#pragma once
#include "../Utils/Vector3.h"
#include "Mob.h"
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
class Sheep : public Mob {
private:

	float rotDavDr = 0;
	float rotDR = 1;

	float eyesOffset = 0.5f; //Alçada ulls

public:
	Sheep(World* world, Vector3<float> pos);

	void update(double delta);

	void draw(double delta);
	void destroy();

	void setCam(Camera* cam);
};

