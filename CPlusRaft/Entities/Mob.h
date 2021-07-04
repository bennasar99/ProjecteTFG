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
	Entitat mob
*/
class Mob : public Entity {
protected:

	int health;
	float rotCounter = 0;
	short rotAct = 0;

	float eyesOffset = 0.5f; //Alçada ulls

public:
	Mob(World* world, Vector3<float> pos, float height);

	void update(double delta);

	void destroy();

	void setCam(Camera* cam);
};

