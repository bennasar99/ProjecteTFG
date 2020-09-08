#pragma once
#include "../Vector3.h"
#include "Entity.h"
#include <list>
#include "../KeyboardManager.h"
#include "../SoundManager.h"
#include "../Light.h"
#include "../Object3D.h"

class Block;
class World;

/*
	Entitat jugador
*/
class Player : public Entity {
private:
	float grav = 1;
	Vector3 vel; //Velocitat

	int health;
	float rot = 0;

	float eyesOffset = 0.5f; //Al�ada ulls

	//Inventari *inv;

	int gamemode = 1; //0 = creative, 1 = survival

public:
	Player(World* world, Vector3 pos);

	void update(int delta);
	void control(unsigned char key);
	void control(int delta, Camera *cam);

	void draw();
	void destroy();

	void setCam(Camera* cam);

	void onAttach();
	void onDeattach();
};
