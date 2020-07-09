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

class Grua : public Entity {
private:

	Entity* ent; //Entitat agafada per la grua
	Block* bloc; //Bloc agafat per la grua

	Vector3 hookPos; //Posició del ganxo

	//Desplaçament del ganxo a la grua
	float hookY = 0;
	float hookX = 0;

	//Parts de la grua
	Object3D base;
	Object3D carrier;
	Object3D crane1;
	Object3D crane2;
	Object3D hook;

public:
	Grua(World* world, Vector3 pos);

	void update(int delta);
	void control(unsigned char key);
	void control(int delta);

	void draw();
	void destroy();

	void setCam(Camera* cam);
};

