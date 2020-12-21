#pragma once
#include "../Vector3.h"
#include "Entity.h"
#include <list>
#include "../KeyboardManager.h"
#include "../SoundManager.h"
#include "../Light.h"

class Block;
class World;

/*
	Entitat jugador
*/
class Player : public Entity {
private:
	float grav = 1;
	float speed = 1; 

	int health;
	float rot = 0;

	float eyesOffset = 0.5f; //Alçada ulls

	//Inventari *inv;

	int gamemode = 0; //0 = creative, 1 = survival

public:
	Player(World* world, Vector3<float> pos);

	void update(int delta);
	void control(int key);
	void control(int delta, Camera *cam);

	void draw();
	void destroy();

	void setCam(Camera* cam);

	void onAttach();
	void onDeattach();
};

