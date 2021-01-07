#pragma once
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Vector3.h"
#include <stdlib.h>  
#include <string.h>
#include <list>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <future> 

#include "Light.h"
#include "Blocks/Block.h"
#include "Blocks/LightBlock.h"
#include "Blocks/Jukebox.h"
#include "Blocks/EntityBlock.h"
#include "Blocks/SolidBlock.h"
#include "Blocks/LiquidBlock.h"
#include "Blocks/SpreadBlock.h"
#include "Entities/Player.h"
#include <algorithm>
#include "Utils.h"
#include "Camera.h"
#include "Entities/Entity.h"
#include "Chunk.h"
#include "Blocks//BlockRenderer.h"
#include "Generation/WorldGenerator.h"
#include "lib/FastNoiseLite.h"
#include "ryml.hpp"

#define REGIONSIZE 16
#define CORES 4

class Block;
class LightBlock;
class Pendul;
enum class Bloc;

enum class ChunkState {
	LLEST,
	BLOQUEJAT,
	TERRENY,
	PENDENT,
	BUIT
};

class World {
private:

	std::future<Chunk*> cnk[CORES];

	WorldGenerator wGen;

	Chunk **chunks;
	ChunkState *estat;
	short pendents = 0;

	int sol = 0;
	float daytime = 0;
	Vector3<float> solpos;

	Vector3<int> minpos;
	Vector3<int> maxpos = Vector3<int>(0, 0, 0);

	std::list<Light*> lights;
	std::list<Entity*> entities;
	std::list<Vector3<int>> vChunks; //Llista de chunks visibles

	int getDesp(Vector3<int> pos);
	Vector3<int> spawn; //Spawn point

	int seed;
	int updTimer = 200;

	FastNoiseLite noise;
	std::string name;

	void updateGeneration();

public:
	//Han de ser nombres parells
	Vector3<int> size;

	Camera* camera;
	BlockRenderer *br;

	void drawBloc(Bloc tipus);

	bool setBlock(Bloc tipus, Vector3<int> pos);
	bool setBlock(Bloc tipus, Vector3<int> pos, Block* parent, bool listUpdate);
	bool setBlock(Block* bloc, Vector3<int> pos, bool listUpdate);
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
	void setSol(int sol);
	void drawSol(Vector3<float> pos, float dist); //Respecte una posici� i a una dist�ncia determinada

	//Chunks
	void updateNeighborChunks(Vector3<int> cpos, Vector3<int> bpos);

	void update(int delta, Vector3<float> camPos);

	//void generate(int seed);
	void save();

	void drawMap(float scrAspect, Entity* ent, int mapY);
	void updateVisibility();

	Vector3<int> getRegion(Vector3<int> cPos);
	bool saveRegion(Vector3<int> rPos);
	bool loadRegion(Vector3<int> rPos);

	World(std::string name, int seed, int sizex, int sizey, int sizez, Camera* camera);
	World(std::string file, Camera* camera);
	World();
};