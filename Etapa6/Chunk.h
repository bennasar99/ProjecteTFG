#pragma once

#include "Blocks/Block.h"
#define CHUNKSIZE 16

class Chunk {
private:
	Block* blocs[CHUNKSIZE][CHUNKSIZE][CHUNKSIZE];
	Vector3 cpos;

	GLuint tlist;
	GLuint olist;

	World* world;

	bool firstdraw;

	//GLuint vbo;
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

	void updateDL();

	int nblocs;

	Chunk(World* world, Vector3 pos);
};