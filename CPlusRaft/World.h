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

#include "Light.h"
#include "Blocks/Block.h"
#include "Blocks/LightBlock.h"
#include "Blocks/Jukebox.h"
#include "Blocks/EntityBlock.h"
#include "Blocks/SolidBlock.h"
#include "Blocks/LiquidBlock.h"
#include "Blocks/SpreadBlock.h"
#include "Entities/Player.h"
#include "Utils.h"
#include "Camera.h"
#include "Entities/Entity.h"
#include "Chunk.h"
#include "Generation/WorldGenerator.h"
#include "lib/FastNoiseLite.h"
#include "ryml.hpp"
#include "Utils/Vector3.h"
#include "Utils/ThreadManager.h"
#include "lib/zlib/zlib.h"

using namespace std;
#define REGIONSIZE 16
#define SPAWNSIZE 100
#define DAYTIME 120 //Durada del dia (segons)

class Block;
class LightBlock;
class Pendul;

enum class ChunkState {
	BUIT,
	LLEST,
	TERRENY,
	PENDENT,
	CARREGAT
};

enum class RegionState {
	BUIDA,
	DIRTY,
	CARREGADA
};

class World {
private:

	double lastTime;

	int genCores;
	std::vector< std::future<Chunk*> > cnk;

	WorldGenerator wGen;

	//Chunk** chunks;
	struct Vector3Compare
	{
		bool operator() (const Vector3<int>& lhs, const Vector3<int>& rhs) const
		{
			return std::make_tuple(lhs.x, lhs.y, lhs.z) < std::make_tuple(rhs.x, rhs.y, rhs.z);
		}
	};
	map<Vector3<int>, Chunk*, Vector3Compare> chunks;
	map<Vector3<int>, ChunkState, Vector3Compare> cestat;
	map<Vector3<int>, bool, Vector3Compare> rloaded; //True si ja s'ha intentat carregar la regió de fixer
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
	std::mutex mutex;

	void updateGeneration();
	Vector3<int> getChunkPos(Vector3<int> bpos);

public:
	//bool checkthreads = false;

	Camera* camera;

	void drawBloc(Bloc tipus);

	bool setBlock(Bloc tipus, Vector3<int> pos, bool overwrite = true, bool listUpdate = true);
	bool setBlock(Block* bloc, Vector3<int> pos, bool listUpdate = true);
	Bloc getBlock(Vector3<int> pos);
	Bloc getBlock(Vector3<float> pos);
	Block* getBlockPointer(Vector3<int> pos, bool remove);
	bool deleteBlock(Vector3<int> pos, bool destroy);

	void interact(Vector3<int> pos);

	void destroy();

	Vector3<int> getSpawn();

	void drawAxis(Vector3<float> pos, float axisSize);
	void draw(Vector3<float> pos, float dist);

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

	void update(float delta, Vector3<float> camPos);

	/*void setRegionDirty(Vector3<int> rPos);*/

	//void generate(int seed);
	void save();

	void drawMap(float scrAspect, Entity* ent, int mapY);
	void updateVisibility();

	Vector3<int> getRegion(Vector3<int> cPos);
	bool saveRegion(Vector3<int> rPos);
	bool loadRegion(Vector3<int> rPos);
	bool loadFile(std::string path);

	void redrawChunks();

	World(std::string name, int seed, int sizex, int sizey, int sizez, Camera* camera);
	World(std::string file, Camera* camera);
	World();
};