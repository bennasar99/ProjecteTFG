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
	Entita cotxe
*/
class Car : public Entity {
private:

	int model = 0; //Model del cotxe, 0 = blender, 1 = dibuixat propi

	float acc = 0; //Acceleració
	Vector3 vel; //Velocitat

	//Llums
	Light* llumdret;
	Light* llumesq;;

	//Objecte (mesh)
	Object3D carMesh;
	
	bool enMarxa = false;

public:
	Car(World* world, Vector3 pos, int model);

	void update(int delta);
	void control(unsigned char key);
	void control(int delta);

	void updateLights();

	void lightsOn(bool lights);
	bool getLightsOn();

	void draw();
	void destroy();

	void setCam(Camera* cam);

	void onAttach();
	void onDeattach();
};

