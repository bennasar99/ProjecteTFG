#pragma once
#include "../TextureManager.h"
#include "../Camera.h"

class World;

enum class Entitat {
	JUGADOR,
};

class Entity {
protected:

	World* world; //M�n de l'entitat
	Vector3<float> pos; //Posici� de l'entitat
	float rot = 0; //Rotaci� (eix Y) de l'entitat

	bool controllable = false; //Indica si l'entitat �s controlable pel jugador

public:

	virtual void draw();
	virtual void update(int delta); //Actualitzacions passives
	virtual void destroy();

	virtual void control(unsigned char key); //Control actiu (entitat controlada i tecla pitjada)
	virtual void control(int delta); //Actualitzacions actives (entitat controlada)
	virtual void control(int delta, Camera *cam); //Actualitzacions actives (entitat controlada)

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