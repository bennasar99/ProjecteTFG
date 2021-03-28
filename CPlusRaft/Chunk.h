#pragma once

#include "Blocks/Block.h"
#include "Render/ChunkMesh.h"
#include "Utils/Vector3.h"
#include "Generation/WorldGenerator.h"
#include <list>
#include <mutex>
#define CHUNKSIZE 16

class Chunk {
private:
	//Block* blocs[CHUNKSIZE][CHUNKSIZE][CHUNKSIZE] = {};
	//int height[CHUNKSIZE][CHUNKSIZE]; //Heightmap
	Bloc blocs[CHUNKSIZE][CHUNKSIZE][CHUNKSIZE] = {};
	

	Bioma bio;

	Vector3<int> cpos;

	World* world;

	ChunkMesh cMesh;

	int lastYupd = 0;

	class dT {
	public:
		Vector3<float> pos;
		bool visible[6] = { false, false, false, false, false, false };
		dT(Vector3<float> pos) {
			this->pos = pos;
		};
	};
	std::list<dT> transparent = {};

	std::mutex mutex;

public:

	bool firstdraw = true;
	bool dirty = false;

	void drawO();
	void drawT();

	Vector3<int> getPos();

	bool setBlock(Bloc bloc, Vector3<int> bpos, bool overwrite = true);
	bool delBlock(Vector3<int> bpos, bool destroy);
	Bloc getBlock(Vector3<int> bpos);

	void interact(Vector3<int> bpos);
	void update(float delta);

	bool isVisible(Vector3<int> bpos);
	Bloc getBlockWorld(Vector3<int> bpos);
	bool setBlockWorld(Bloc bloc, Vector3<int> bpos);

	bool getByteData(char* arr);
	bool readFromByteData(char* arr);

	Bioma getBiome();
	void setBiome(Bioma biome);

	void updateMesh();
	void updateTransparency(Vector3<float> pPos);

	void setDirty(bool dirty);
	bool getDirty();

	int nblocs;

	Chunk(World* world, Vector3<int> pos);
	~Chunk();
};