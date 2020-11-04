#pragma once

#include "Blocks/Block.h"
#include "Render/ChunkMesh.h"
#include "Vector3.h"
#define CHUNKSIZE 16

class Chunk {
private:
	Block* blocs[CHUNKSIZE][CHUNKSIZE][CHUNKSIZE];
	Vector3 cpos;

	World* world;

	bool firstdraw;

	ChunkMesh *cMesh;

public:
	void drawO();
	void drawT();

	Vector3 getPos();

	bool setBlock(Block* bloc, Vector3 bpos);
	bool delBlock(Vector3 bpos, bool destroy);
	Bloc getBlock(Vector3 bpos);

	void interact(Vector3 bpos);
	void update(int delta);
	void destroy();

	bool isVisible(Vector3 bpos);
	Bloc getBlockWorld(Vector3 bpos);

	bool getByteData(char* arr);
	bool readFromByteData(char* arr);

	void updateMesh();

	int nblocs;

	Chunk(World* world, Vector3 pos);
};