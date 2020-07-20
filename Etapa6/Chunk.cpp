#include <GL/glew.h>
#include "Chunk.h"

Chunk::Chunk() {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				blocs[x][y][z] = 0;
			}
		}
	}
}

void Chunk::draw() {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				blocs[x][y][z]->draw();
			}
		}
	}
}

Vector3 Chunk::getPos() {
	return cpos;
}

bool Chunk::setBlock(Block* bloc, Vector3 pos) {
	//Hi ha blocs amb les seves pròpies classes, sinó s'utilitza la classe genèrica
	this->blocs[(int)pos.x][(int)pos.y][(int)pos.z] = bloc;
	return true;
}

void Chunk::update(int delta) {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				blocs[x][y][z]->update(delta);
			}
		}
	}
}