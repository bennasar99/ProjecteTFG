#pragma once

#pragma once
#include "../Vector3.h"
#include "Entity.h"
#include <list>

class World;

/*
	Noria articulada
*/
class Noria : public Entity {
private:

	float radi; //Radi de la nòria
	float angle; //Rotació
	float velocitat;
	float vagons; //Nombre de vagons
	float midaCabina; //mida d'una cabina

	bool isFather = false;

	void dibuixaCabina(float mida);

public:
	Noria(World* world, Vector3 pos, float radi, float velocitat, float vagons, float midaCabina);

	void update(int delta);

	void draw();
	void destroy();
};

