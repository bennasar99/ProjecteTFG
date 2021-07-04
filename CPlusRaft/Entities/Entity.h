#pragma once
#include "../Utils/TextureManager.h"
#include "../Camera.h"

class World;

enum class Entitat : uint8_t {
	JUGADOR,
	OVELLA,
	ESTRUC
};

class Entity {
protected:

	float height = 1.0f;

	float grav = 0.0f;

	World* world; //Món de l'entitat
	Vector3<float> pos; //Posició de l'entitat
	Vector3<float> speed = Vector3<float>(0,0,0); //Velocitat de l'entitat
	float speedM = 1.0f; //Multiplicador de velocitat
	float rot = 0; //Rotació (eix Y) de l'entitat

	bool controllable = false; //Indica si l'entitat és controlable pel jugador
	bool checkCollisions = true; //Indica si l'entitat ha d'estar subjecte a col·lisions

public:

	inline static float GRAVMAX = 16.0f;
	inline static float GRAVITY = 9.8f;

	virtual void draw(double delta);
	virtual void update(double delta); //Actualitzacions passives
	virtual void destroy();

	virtual void control(int key); //Control actiu (entitat controlada i tecla pitjada)
	virtual void control(double delta); //Actualitzacions actives (entitat controlada)
	virtual void control(double delta, Camera *cam); //Actualitzacions actives (entitat controlada)

	virtual void onAttach(); //Es crida quan l'usuari agafa el control de l'entitat
	virtual void onDeattach(); //Es crida quan l'usuari deixa de controlar l'entitat

	virtual void setCam(Camera* cam);

	Vector3<float> getPos();
	float getRot();
	void setRot(float rot);
	void setPos(Vector3<float> pos);
	bool getControllable();

	Entity(World* world, Vector3<float> pos, float height);
	Entity();
};