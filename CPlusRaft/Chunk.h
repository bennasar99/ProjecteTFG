#pragma once

#include "Blocks/Block.h"
#include "Render/ChunkMesh.h"
#include "Vector3.h"
#define CHUNKSIZE 16

class Chunk {
private:
	Block* blocs[CHUNKSIZE][CHUNKSIZE][CHUNKSIZE];
	Vector3<int> cpos;

	World* world;

	bool firstdraw;

	ChunkMesh *cMesh;

public:
	void drawO();
	void drawT();

	Vector3<int> getPos();

	bool setBlock(Block* bloc, Vector3<int> bpos);
	bool delBlock(Vector3<int> bpos, bool destroy);
	Bloc getBlock(Vector3<int> bpos);

	void interact(Vector3<int> bpos);
	void update(int delta);
	void destroy();

	bool isVisible(Vector3<int> bpos);
	Bloc getBlockWorld(Vector3<int> bpos);

	bool getByteData(char* arr);
	bool readFromByteData(char* arr);

	void updateMesh();

	int nblocs;

	Chunk(World* world, Vector3<int> pos);
};