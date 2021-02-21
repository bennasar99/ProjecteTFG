#pragma once
#include "../Utils/Vector3.h"
#include "Block.h"
#include "../Chunk.h"
#include "../Utils/SoundManager.h"

class World;



/*
	Classe que representa un bloc amb llum vinculada
*/
class SolidBlock : public Block {
private:

public:
	SolidBlock(Bloc id);

	void draw(ChunkMesh *cM, bool visible[6], Vector3<int> relPos);
	void drawMarching(ChunkMesh* cM, bool visible[6], Vector3<int> relPos, Vector3<int> cPos, World *world, Chunk *cnk);

};
