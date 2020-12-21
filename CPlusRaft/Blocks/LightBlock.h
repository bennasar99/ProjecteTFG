#pragma once
#include "Block.h"
#include "../Light.h"
#include "../SoundManager.h"
#include "../Vector3.h"

class World;

/*
	Classe que representa un bloc amb llum vinculada
*/
class LightBlock : public Block {
private:

	Light* light; //Llum vinculada

	Vector3<int> pos; //Posició del bloc

public:
	LightBlock(World* world, Bloc id, Vector3<int> pos);

	void destroy();

	void draw();

	void interact();

};
