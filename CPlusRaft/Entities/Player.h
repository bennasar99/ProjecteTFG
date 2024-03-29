#pragma once
#include "../Utils/Vector3.h"
#include "Entity.h"
#include <list>
#include "../Utils/KeyboardManager.h"
#include "../Utils/SoundManager.h"
#include "../Light.h"

class Block;
class World;

/*
	Entitat jugador
*/
class Player : public Entity {
private:
	float speedM = 1;

	int health;
	float rot = 0;

	float eyesOffset = 0.5f; //Al�ada ulls

	//Inventari *inv;

	int gamemode = 0; //0 = creative, 1 = survival

public:

	Player(World* world, Vector3<float> pos);

	void update(double delta);
	void control(int key);
	void control(double delta, Camera *cam);

	void draw(double delta);
	void destroy();

	void setCam(Camera* cam);

	void onAttach();
	void onDeattach();
};

