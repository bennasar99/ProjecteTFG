#pragma once
#include "../Vector3.h"
#include "Block.h"
#include "../SoundManager.h"

class World;

/*
	Classe que representa un bloc amb llum vinculada
*/
class SolidBlock : public Block {
private:

	Vector3<float> pos; //Posició del bloc

public:
	SolidBlock(World* world, Bloc id);

	void destroy();

	void draw(ChunkMesh *cM, bool visible[6], Vector3<float> relPos);

	void interact();

};
