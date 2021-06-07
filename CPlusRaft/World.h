#pragma once
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>  
#include <string.h>
#include <list>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <future> 
#include <algorithm>
#include <map>
#include <concurrent_unordered_map.h>

#include "Light.h"
#include "Blocks/Block.h"
#include "Entities/Player.h"
#include "Utils.h"
#include "Camera.h"
#include "Entities/Entity.h"
#include "Chunk.h"
#include "Generation/WorldGenerator.h"
#include "lib/FastNoiseLite.h"
#include "ryml.hpp"
#include "Utils/Vector3.h"
#include "Utils/RenderManager.h"
#include "Utils/ThreadManager.h"
#include "lib/zlib/zlib.h"
#include "Entities/Sheep.h"
#include "Entities/Ostrich.h"

#include "ChunkManager.h"

using namespace std;
#define REGIONSIZE 16
#define SPAWNSIZE 100
#define DAYTIME 120 //Durada del dia (segons)

class Block;
class LightBlock;
class Pendul;

class World {
private:
	std::shared_mutex sMutex;
	double lastTime;

	int genCores;
	std::vector< std::future<Chunk*> > cnk;
	std::vector< std::future<bool> > regLoad;

	WorldGenerator wGen;
	ChunkManager cM;

	struct v3_hash {
		std::size_t operator()(const Vector3<int>& vec) const {
			return (size_t)vec.x + 128 * ((size_t)vec.y + 128 * (size_t)vec.z);
		}
	};

	concurrency::concurrent_unordered_map<Vector3<int>, Chunk*, v3_hash> chunks;
	short pendents = 0;

	int sol = 0;
	float daytime = 0;
	Vector3<float> solpos;

	Vector3<int> minpos;
	Vector3<int> maxpos = Vector3<int>(0, 0, 0);

	std::list<Light*> lights;
	std::list<Entity*> entities;
	std::list<Vector3<int>> vChunks; //Llista de chunks visibles

	Vector3<int> spawn; //Spawn point

	int seed;
	int updTimer = 200;

	std::string name;
	std::mutex mutex; //mutex per la llista de chunks visibles
	std::mutex uMutex; //mutex per eliminar chunks del mapa "chunks"

	void updateGeneration();

public:
	//bool checkthreads = false;
	Vector3<int> getChunkPos(Vector3<int> bpos);
	Camera* camera;

	void drawBloc(Bloc tipus);

	bool setBlock(Bloc tipus, Vector3<int> pos, bool overwrite = true, bool listUpdate = true);
	Bloc getBlock(Vector3<int> pos);
	Bloc getBlock(Vector3<float> pos);
	Block* getBlockPointer(Vector3<int> pos, bool remove);
	bool deleteBlock(Vector3<int> pos, bool destroy);

	void interact(Vector3<int> pos);

	void destroy();

	Vector3<int> getSpawn();

	void drawAxis(Vector3<float> pos, float axisSize);
	void draw(Vector3<float> pos, float dist, double delta);

	//Llums
	void drawLights();
	void setLight(int lightnum, Light* light);
	Light* addLight(Vector3<float> pos);
	void delLight(Light* light);

	//Entitats
	Entity* addEntity(Entitat ent, Vector3<float> pos);
	void delEntity(Entity* ent);
	Entity* getNearestEntity(Vector3<float> pos, float range, bool controllable);

	//Sol
	//void setSol(int sol);
	void drawSol(Vector3<float> pos, float dist); //Respecte una posició i a una distància determinada

	//Chunks
	void updateNeighborChunks(Vector3<int> cpos, Vector3<int> bpos);
	void updateNeighborChunks(Vector3<int> cpos);

	void update(double delta, Vector3<float> camPos);
	bool setRandomSpawn();

	/*void setRegionDirty(Vector3<int> rPos);*/

	//void generate(int seed);
	void save();

	void drawMap(float scrAspect, Entity* ent, int mapY, int range);
	void updateVisibility();

	Vector3<int> getRegion(Vector3<int> cPos);
	bool doRegion(Vector3<int> rPos, bool save = true, bool unload = true);
	bool loadRegion(Vector3<int> rPos);
	bool loadFile(std::string path);
	bool safeMod(Vector3<int> cPos);

	void redrawChunks();
	Chunk* getChunk(Vector3<int> cPos);

	World(std::string name, int seed, Camera* camera);
	World(std::string file, Camera* camera);
};