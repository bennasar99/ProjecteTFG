#pragma once
#include "../Vector3.h"
#include "Block.h"
#include "../SoundManager.h"

class World;

/*
	Classe que representa un bloc amb llum vinculada
*/
class LiquidBlock : public Block {
private:

	Vector3 pos; //Posici� del bloc

public:
	LiquidBlock(World* world, Bloc id);

	void destroy();

	void draw(ChunkMesh *cM, bool visible[6], Vector3 relPos);

	void interact();

};
