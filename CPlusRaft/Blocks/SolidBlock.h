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

public:
	SolidBlock(Bloc id);

	void draw(ChunkMesh *cM, bool visible[6], Vector3<int> relPos);

};
