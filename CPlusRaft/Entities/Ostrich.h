#pragma once
#include "../Utils/Vector3.h"
#include "Entity.h"
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
class Ostrich : public Entity {
private:
	int anim = 0;

	Estat estat = Estat::CAMINANT;
	int frameInici[4] = { 0, 65, 130, 180 };
	int frameCount[4] = { 60, 60, 45, 50 };

	float grav = 1;
	float gravmax = 100;
	float speed = 16.0f;

	bool firstdraw = false;

	Mesh cos;
	Mesh davDR;
	Mesh davES;
	Mesh darDR;
	Mesh darES;

	int health;
	float rotCounter = 0;
	float rot = 0;
	float rotDavDr = 0;
	float rotDR = 1;
	short rotAct = 0;

	float eyesOffset = 0.5f; //Alçada ulls

public:
	Ostrich(World* world, Vector3<float> pos);

	void update(float delta);

	void draw();
	void destroy();

	void setCam(Camera* cam);
};

