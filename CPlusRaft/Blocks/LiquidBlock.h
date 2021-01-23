#pragma once
#include "../Utils/Vector3.h"
#include "Block.h"
#include "../Utils/SoundManager.h"

class World;

/*
	Classe que representa un bloc amb llum vinculada
*/
class LiquidBlock : public Block {
private:
	int lvl;
	Vector3<int> pos; //Posició del bloc

public:
	LiquidBlock(Bloc id, Vector3<int> pos);
	LiquidBlock(Bloc id, Vector3<int> pos, int lvl);

	void destroy(World* world);

	void draw(ChunkMesh *cM, bool visible[6], Vector3<int> relPos);

	void interact(World* world);

	void update(int delta, World* world);

	void setLvl(int lvl);

};
