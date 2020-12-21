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

#include "Light.h"
#include "Blocks/Block.h"
#include "Blocks/LightBlock.h"
#include "Blocks/Jukebox.h"
#include "Blocks/EntityBlock.h"
#include "Blocks/SolidBlock.h"
#include "Blocks/LiquidBlock.h"
#include "Blocks/SpreadBlock.h"
#include <algorithm>
#include "Utils.h"
#include "Camera.h"
#include "Entities/Entity.h"
#include "Chunk.h"
#include "Blocks//BlockRenderer.h"

#include "lib/FastNoiseLite.h"
#include "ryml.hpp"

class Block;
class LightBlock;
class Pendul;
enum class Bloc;

class World {
private:

	Chunk **chunks;
	int sol = 0;
	float daytime = 0;
	Vector3<float> solpos;

	Vector3<int> minpos;
	Vector3<int> maxpos = Vector3<int>(0, 0, 0);

	std::list<Light*> lights;
	std::list<Entity*> entities;

	int getDesp(Vector3<int> pos);
	Vector3<int> spawn; //Spawn point

	int seed;
	int updTimer = 1000;

	FastNoiseLite noise;

public:
	//Han de ser nombres parells
	int sizex;
	int sizey;
	int sizez;

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
	void updateLights(Vector3<float> camPos, Vector3<float> front, float camFov, float aspect);
	void setLight(int lightnum, Light* light);
	Light* addLight(Vector3<float> pos);
	void delLight(Light* light);

	//Entitats
	Entity* addEntity(Entitat ent, Vector3<float> pos);
	void delEntity(Entity* ent);
	Entity* getNearestEntity(Vector3<float> pos, float range, bool controllable);

	//Sol
	void setSol(int sol);
	void drawSol(Vector3<float> pos, float dist); //Respecte una posició i a una distància determinada

	//Chunks
	void updateNeighborChunks(Vector3<int> cpos, Vector3<int> bpos);

	void update(int delta, Vector3<float> camPos);

	void generate(int seed);
	void save(std::string file);

	World(int seed, int sizex, int sizey, int sizez, Camera* camera);
	World(std::string file, Camera* camera);
	World();
};