#pragma once
#include "../Vector3.h"
#include "Block.h"
#include "../Light.h"
#include "../SoundManager.h"

class World;

/*
	Classe que representa un bloc amb llum vinculada
*/
class LightBlock : public Block {
private:

	Light* light; //Llum vinculada

	Vector3 pos; //Posició del bloc

public:
	LightBlock(World* world, Bloc id, Vector3 pos);

	void destroy();

	void draw();

	void interact();

};
