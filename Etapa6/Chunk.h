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

	bool setBlock(Block* bloc, Vector3 pos);
	void delBlock(Block);
	void update(int delta);
};