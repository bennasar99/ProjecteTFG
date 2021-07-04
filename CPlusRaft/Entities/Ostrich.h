#pragma once
#include "../Utils/Vector3.h"
#include "Mob.h"
#include <list>
#include "../Utils/KeyboardManager.h"
#include "../Utils/SoundManager.h"
#include "../Light.h"
#include "../Render/Mesh.h"
#include "../Utils/ModelManager.h"

class Block;
class World;

enum class Estat {
	ATURAT,
	ATACANT,
	MENJANT,
	CAMINANT
};

/*
	Entitat jugador
*/
class Ostrich : public Mob {
private:
	float anim = 0;

	Estat estat = Estat::CAMINANT;
	int frameInici[4] = { 0, 65, 130, 180 };
	int frameCount[4] = { 60, 60, 45, 50 };


	int health;

	float canviEstatCounter;

	float eyesOffset = 0.5f; //Alçada ulls

public:
	Ostrich(World* world, Vector3<float> pos);

	void update(double delta);

	void draw(double delta);
	void destroy();

	void setCam(Camera* cam);
};

