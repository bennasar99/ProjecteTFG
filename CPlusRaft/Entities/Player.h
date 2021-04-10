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
	float gravmax = 100;
	float speed = 16.0f;

	int health;
	float rot = 0;

	float eyesOffset = 0.5f; //Alçada ulls

	//Inventari *inv;

	int gamemode = 0; //0 = creative, 1 = survival

public:
	float grav = 1;

	Player(World* world, Vector3<float> pos);

	void update(float delta);
	void control(int key);
	void control(float delta, Camera *cam);

	void draw();
	void destroy();

	void setCam(Camera* cam);

	void onAttach();
	void onDeattach();
};

