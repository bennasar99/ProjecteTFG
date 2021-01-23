#pragma once
#include "../Utils/Vector3.h"
#include "Block.h"

class World;

/*
	Classe que representa un bloc amb llum vinculada
*/
class SpreadBlock : public Block {
private:

	Vector3<int> pos; //Posició del bloc
	int growTimer;

public:
	SpreadBlock(Bloc id, Vector3<int> pos);

	void destroy(World* world);

	void draw(ChunkMesh *cM, bool visible[6], Vector3<int> relPos);

	void interact(World* world);

	void update(int delta, World* world);

};
