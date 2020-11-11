#pragma once
#include "../Vector3.h"
#include "Block.h"

class World;

/*
	Classe que representa un bloc amb llum vinculada
*/
class SpreadBlock : public Block {
private:

	Vector3 pos; //Posició del bloc

public:
	SpreadBlock(World* world, Bloc id);

	void destroy();

	void draw(ChunkMesh *cM, bool visible[6], Vector3 relPos);

	void interact();

};
