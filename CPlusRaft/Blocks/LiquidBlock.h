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

	Vector3<int> pos; //Posició del bloc

public:
	LiquidBlock(Bloc id);

	void destroy(World* world);

	void draw(ChunkMesh *cM, bool visible[6], Vector3<int> relPos);

	void interact(World* world);

};
