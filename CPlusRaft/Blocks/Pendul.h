#pragma once
#include "../Vector3.h"
#include "Block.h"
#include <list>

class Block;
class World;

/*
	Pèndul doble 3D (moviment 2D)
*/
class Pendul : public Block {
private:

	Vector3 pos; //Posició del pèndul

	// dades del pènduls
	float l1;
	float angle1;
	float w1;
	float m1;
	float a1;

	float l2;
	float angle2;
	float w2;
	float m2;
	float a2;

	// punts anteriors
	std::list<Vector3> punts;
	bool penabled = true;

public:
	Pendul(World* world, Vector3 pos);

	void update(int delta);
	void togglePoints();

	void draw(bool wireframe);
	void destroy();
};