#pragma once
#include "../Utils/TextureManager.h"
#include "../Camera.h"

class World;

enum class Entitat {
	JUGADOR,
};

class Entity {
protected:

	World* world; //Món de l'entitat
	Vector3<float> pos; //Posició de l'entitat
	float rot = 0; //Rotació (eix Y) de l'entitat

	bool controllable = false; //Indica si l'entitat és controlable pel jugador

public:

	virtual void draw();
	virtual void update(float delta); //Actualitzacions passives
	virtual void destroy();

	virtual void control(int key); //Control actiu (entitat controlada i tecla pitjada)
	virtual void control(float delta); //Actualitzacions actives (entitat controlada)
	virtual void control(float delta, Camera *cam); //Actualitzacions actives (entitat controlada)

	virtual void onAttach(); //Es crida quan l'usuari agafa el control de l'entitat
	virtual void onDeattach(); //Es crida quan l'usuari deixa de controlar l'entitat

	virtual void setCam(Camera* cam);

	Vector3<float> getPos();
	float getRot();
	void setRot(float rot);
	void setPos(Vector3<float> pos);
	bool getControllable();

	Entity(World* world, Vector3<float> pos);
	Entity();
};