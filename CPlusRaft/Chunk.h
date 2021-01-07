#pragma once

#include "Blocks/Block.h"
#include "Render/ChunkMesh.h"
#include "Vector3.h"
#include "Generation/WorldGenerator.h"
#define CHUNKSIZE 16

class Chunk {
private:
	Block* blocs[CHUNKSIZE][CHUNKSIZE][CHUNKSIZE];
	//int height[CHUNKSIZE][CHUNKSIZE]; //Heightmap

	Bioma bio;

	Vector3<int> cpos;

	World* world;

	ChunkMesh *cMesh;

	int lastYupd = 0;

public:
	bool firstdraw = true;

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
	bool setBlockWorld(Block* bloc, Vector3<int> bpos);

	bool getByteData(char* arr);
	bool readFromByteData(char* arr);

	Bioma getBiome();
	void setBiome(Bioma biome);

	void updateMesh();

	int nblocs;

	Chunk(World* world, Vector3<int> pos);
};