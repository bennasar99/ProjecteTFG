#pragma once
#include "../TextureManager.h"
#include "../Camera.h"

class World;

enum class Entitat {
	COTXE,
	COTXE2,
	GRUA,
	NORIA
};

class Entity {
protected:

	World* world; //Món de l'entitat
	Vector3 pos; //Posició de l'entitat
	float rot = 0; //Rotació (eix Y) de l'entitat

	bool controllable = false; //Indica si l'entitat és controlable pel jugador

public:

	virtual void draw();
	virtual void update(int delta); //Actualitzacions passives
	virtual void destroy();

	virtual void control(unsigned char key); //Control actiu (entitat controlada i tecla pitjada)
	virtual void control(int delta); //Actualitzacions actives (entitat controlada)

	virtual void onAttach(); //Es crida quan l'usuari agafa el control de l'entitat
	virtual void onDeattach(); //Es crida quan l'usuari deixa de controlar l'entitat

	virtual void setCam(Camera* cam);

	Vector3 getPos();
	float getRot();
	void setPos(Vector3 pos);
	bool getControllable();

	Entity(World* world, Vector3 pos);
	Entity();
};