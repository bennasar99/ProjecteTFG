#pragma once
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Vector3.h"
#include <stdlib.h>  
#include <string.h>
#include <list>
#include "Light.h"
#include "Blocks/Block.h"
#include "Blocks/LightBlock.h"
#include "Blocks/SurfaceBlock.h"
#include "Blocks/Pendul.h"
#include "Blocks/Jukebox.h"
#include "Blocks/Mirror.h"
#include "Blocks/EntityBlock.h"
#include <algorithm>
#include "Utils.h"
#include "Camera.h"
#include "Entities/Entity.h"
#include "Entities/Car.h"
#include "Entities/Grua.h"
#include "Entities/Noria.h"


class Block;
class LightBlock;
class Pendul;
enum class Bloc;

class World {
private:
	Block **blocs;
	int sol = 0;
	float daytime = 0;
	Vector3 solpos;

	Vector3 minpos;
	Vector3 maxpos = Vector3(0, 0, 0);

	std::list<Light*> lights;
	std::list<Entity*> entities;

	int getDesp(Vector3 pos);

public:
	//Han de ser nombres parells
	int sizex;
	int sizey;
	int sizez;

	Camera* camera;


	bool setBlock(Bloc tipus, Vector3 pos);
	bool setBlock(Bloc tipus, Vector3 pos, Block* parent);
	bool setBlock(Block* bloc, Vector3 pos);
	Bloc getBlock(Vector3 pos);
	Block* getBlockPointer(Vector3 pos, bool remove);
	bool deleteBlock(Vector3 pos, bool destroy);

	void interact(Vector3 pos);

	void destroy();

	void drawAxis(Vector3 pos, float axisSize);
	void drawBloc(Vector3 pos, Bloc tipus, bool wireframe);
	void draw(Vector3 pos, float dist);

	//Llums
	void updateLights(Vector3 camPos, Vector3 front, float camFov, float aspect);
	void setLight(int lightnum, Light* light);
	Light* addLight(Vector3 pos);
	void delLight(Light* light);

	//Entitats
	Entity* addEntity(Entitat ent, Vector3 pos);
	void delEntity(Entity* ent);
	Entity* getNearestEntity(Vector3 pos, float range, bool controllable);

	//Sol
	void setSol(int sol);
	void drawSol(Vector3 pos, float dist); //Respecte una posició i a una distància determinada

	void update(int delta, Vector3 camPos);

	World(int sizex, int sizey, int sizez, Camera* camera);
};