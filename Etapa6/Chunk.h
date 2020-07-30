#pragma once

#include "Blocks//Block.h"

class Chunk {
private:
	Block* blocs[16][16][16];
	Vector3 cpos;

	//GLuint vbo;
public:
	void draw();
	Vector3 getPos();

	bool setBlock(Block* bloc, Vector3 bpos);
	bool delBlock(Vector3 bpos, bool destroy);
	Bloc getBlock(Vector3 bpos);

	void interact(Vector3 bpos);
	void update(int delta);
	void destroy();

	int nblocs;

	Chunk();
};