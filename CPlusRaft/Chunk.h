#pragma once

#include "Blocks/Block.h"
#include "Render/ChunkMesh.h"
#include "Utils/Vector3.h"
#include "Generation/WorldGenerator.h"
#include <list>
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

	class dT {
	public:
		Vector3<float> pos;
		bool visible[6] = { false, false, false, false, false, false };
		dT(Vector3<float> pos) {
			this->pos = pos;
		};
	};
	std::list<dT> transparent;

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

	bool isVisible(Vector3<int> bpos);
	Bloc getBlockWorld(Vector3<int> bpos);
	bool setBlockWorld(Block* bloc, Vector3<int> bpos);

	bool getByteData(char* arr);
	bool readFromByteData(char* arr);

	Bioma getBiome();
	void setBiome(Bioma biome);

	void updateMesh();
	void updateTransparency(Vector3<float> pPos);

	int nblocs;

	Chunk(World* world, Vector3<int> pos);
	~Chunk();
};