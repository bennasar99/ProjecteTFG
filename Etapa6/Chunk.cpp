#include <GL/glew.h>
#include "Chunk.h"
#include "World.h"

Chunk::Chunk(World* world, Vector3 pos) {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				blocs[x][y][z] = nullptr;
			}
		}
	}
	this->world = world;
	this->cpos = pos;
	this->nblocs = 0;
	this->dlist = glGenLists(1);

	this->firstdraw = false;
}

void Chunk::draw() {
	if (nblocs == 0) {
		return;
	}
	if (firstdraw == false) {
		firstdraw = true;
		this->updateDL();
	}
	glCallList(dlist);
}

Vector3 Chunk::getPos() {
	return cpos;
}

bool Chunk::setBlock(Block* bloc, Vector3 pos) {
	//Hi ha blocs amb les seves pròpies classes, sinó s'utilitza la classe genèrica
	if (this->blocs[(int)pos.x][(int)pos.y][(int)pos.z] != 0) {
		this->blocs[(int)pos.x][(int)pos.y][(int)pos.z]->destroy();
		delete this->blocs[(int)pos.x][(int)pos.y][(int)pos.z];
		this->nblocs--;
	}
	this->blocs[(int)pos.x][(int)pos.y][(int)pos.z] = bloc;
	this->nblocs++;

	return true;
}

Bloc Chunk::getBlock(Vector3 pos) {
	if (blocs[(int)pos.x][(int)pos.y][(int)pos.z] == 0) {
		return Bloc::RES;
	}
	return blocs[(int)pos.x][(int)pos.y][(int)pos.z]->getId();
}

void Chunk::update(int delta) {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				if (blocs[x][y][z] != 0) {
					blocs[x][y][z]->update(delta);
				}
			}
		}
	}
}

void Chunk::destroy() {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				if (blocs[x][y][z] != 0) {
					blocs[x][y][z]->destroy();
					delete blocs[x][y][z];
				}
			}
		}
	}
	glDeleteLists(dlist, 1);
}

//TODO actualitzar display list
bool Chunk::delBlock(Vector3 bpos, bool destroy) {
	if (this->blocs[(int)bpos.x][(int)bpos.y][(int)bpos.z] != 0) {
		if (destroy) {
			this->blocs[(int)bpos.x][(int)bpos.y][(int)bpos.z]->destroy();
		}
		delete this->blocs[(int)bpos.x][(int)bpos.y][(int)bpos.z];
		this->blocs[(int)bpos.x][(int)bpos.y][(int)bpos.z] = 0;
		nblocs--;
		printf("%f, %f %f %f \n", bpos.y, cpos.x, cpos.y, cpos.z);
		this->updateDL();
		world->updateNeighborChunks(this->cpos, bpos);
	}
	return true;
}

void Chunk::interact(Vector3 bpos) {
	if (blocs[(int)bpos.x][(int)bpos.y][(int)bpos.z] == 0) {
		return;
	}
	blocs[(int)bpos.x][(int)bpos.y][(int)bpos.z]->interact();
}

void Chunk::updateDL() { //TODO: blocs transparents/translucids
	glDeleteLists(dlist, 1);
	dlist = glGenLists(1);
	glNewList(dlist, GL_COMPILE);

	glTranslatef(0.5f, 0.5f, 0.5f);
	int nb = 0;
	for (int x = 0; x < 16; x++) { //1,15
		for (int z = 0; z < 16; z++) { //1,15
			for (int y = 0; y < 16; y++) { //1,15
				if (blocs[x][y][z] != 0) { //Abans: casos especials (x,y,z == 15,0 a s'or)
					if (this->isVisible(Vector3(x,y,z))) {
						glPushMatrix();
						glTranslatef(x, y, z);
						blocs[x][y][z]->draw();
						glPopMatrix();
					}
					nb++;
					if (nb >= nblocs) { //No cal dibuixar més blocs
						y = 16; z = 16; x = 16;
					}
				}
			}
		}
	}
	glEndList();

	//for (int x = 1; x < 15; x++) { //1,15
	//	for (int z = 1; z < 15; z++) { //1,15
	//		for (int y = 1; y < 15; y++) { //1,15
	//			if (blocs[x][y][z] != 0) { //Abans: casos especials (x,y,z == 15,0 a s'or)
	//				if (blocs[x][y + 1][z] == nullptr || blocs[x][y - 1][z] == nullptr || blocs[x+1][y][z] == nullptr ||
	//					blocs[x-1][y][z] == nullptr || blocs[x][y][z+1] == nullptr || blocs[x][y][z-1] == nullptr) {
	//					glPushMatrix();
	//					glTranslatef(x, y, z);
	//					blocs[x][y][z]->draw();
	//					glPopMatrix();
	//				}
	//				nb++;
	//				if (nb >= nblocs) { //No cal dibuixar més blocs
	//					y = 16; z = 16; x = 16;
	//				}
	//			}
	//		}
	//	}
	//}
	return;

	for (int x = 1; x < 15; x++) {
		for (int z = 1; z < 15; z++) {
			//Blocs de dalt (y = 15)
			int y = 15;
			if (blocs[x][y][z] == 0) {
				continue;
			}
			Vector3 bpos = Vector3(cpos.x * 16.0f + (float)x, (cpos.y+1) * 16.0f, cpos.z * 16.0f + (float)z);
			if (blocs[x][y - 1][z] == nullptr || blocs[x + 1][y][z] == nullptr || blocs[x - 1][y][z] == nullptr || 
				blocs[x][y][z + 1] == nullptr || blocs[x][y][z - 1] == nullptr || this->world->getBlock(bpos) == Bloc::RES){
				glPushMatrix();
				glTranslatef(x, y, z);
				blocs[x][y][z]->draw();
				glPopMatrix();
			}
		}
	}

	for (int x = 1; x < 15; x++) {
		for (int z = 1; z < 15; z++) {
			//Blocs d'abaix (y = 0)
			int y = 0;
			if (blocs[x][y][z] == 0) {
				continue;
			}
			Vector3 bpos = Vector3(cpos.x * 16.0f + (float)x, cpos.y * 16.0f - 1.0f, cpos.z * 16.0f + (float)z);
			if (blocs[x][y + 1][z] == nullptr || blocs[x + 1][y][z] == nullptr || blocs[x - 1][y][z] == nullptr ||
				blocs[x][y][z + 1] == nullptr || blocs[x][y][z - 1] == nullptr || this->world->getBlock(bpos) == Bloc::RES) {
				glPushMatrix();
				glTranslatef(x, y, z);
				blocs[x][y][z]->draw();
				glPopMatrix();
			}
		}
	}

	for (int x = 1; x < 15; x++) {
		for (int y = 1; y < 15; y++) {
			//Blocs de darrera (z = 15)
			int z = 15;
			if (blocs[x][y][z] == 0) {
				continue;
			}
			Vector3 bpos = Vector3(cpos.x * 16.0f + (float)x, cpos.y * 16.0f + (float)y, (cpos.z + 1) * 16.0f);
			if (blocs[x][y - 1][z] == nullptr || blocs[x + 1][y][z] == nullptr || blocs[x - 1][y][z] == nullptr ||
				blocs[x][y+1][z] == nullptr || blocs[x][y][z - 1] == nullptr || this->world->getBlock(bpos) == Bloc::RES) {
				glPushMatrix();
				glTranslatef(x, y, z);
				blocs[x][y][z]->draw();
				glPopMatrix();
			}
		}
	}

	for (int x = 1; x < 15; x++) {
		for (int y = 1; y < 15; y++) {
			//Blocs de davant (z = 0)
			int z = 0;
			if (blocs[x][y][z] == 0) {
				continue;
			}
			Vector3 bpos = Vector3(cpos.x * 16.0f + (float)x, cpos.y * 16.0f + (float)y, cpos.z * 16.0f - 1.0f);
			if (blocs[x][y - 1][z] == nullptr || blocs[x + 1][y][z] == nullptr || blocs[x - 1][y][z] == nullptr ||
				blocs[x][y + 1][z] == nullptr || blocs[x][y][z + 1] == nullptr || this->world->getBlock(bpos) == Bloc::RES) {
				glPushMatrix();
				glTranslatef(x, y, z);
				blocs[x][y][z]->draw();
				glPopMatrix();
			}
		}
	}

	for (int y = 1; y < 15; y++) {
		for (int z = 1; z < 15; z++) {
			//Blocs de dreta (x = 15)
			int x = 15;
			if (blocs[x][y][z] == 0) {
				continue;
			}
			Vector3 bpos = Vector3((cpos.x + 1) * 16.0f, cpos.y * 16.0f + (float)y, cpos.z * 16.0f + (float)z);
			if (blocs[x][y - 1][z] == nullptr || blocs[x][y][z + 1] == nullptr || blocs[x - 1][y][z] == nullptr ||
				blocs[x][y + 1][z] == nullptr || blocs[x][y][z - 1] == nullptr || this->world->getBlock(bpos) == Bloc::RES) {
				glPushMatrix();
				glTranslatef(x, y, z);
				blocs[x][y][z]->draw();
				glPopMatrix();
			}
		}
	}

	for (int y = 1; y < 15; y++) {
		for (int z = 1; z < 15; z++) {
			//Blocs de l'esquerra (x = 0)
			int x = 0;
			if (blocs[x][y][z] == 0) {
				continue;
			}
			Vector3 bpos = Vector3(cpos.x * 16.0f - 1.0f, cpos.y * 16.0f + (float)y, cpos.z * 16.0f + (float)z);
			if (blocs[x][y - 1][z] == nullptr || blocs[x][y][z + 1] == nullptr || blocs[x + 1][y][z] == nullptr ||
				blocs[x][y + 1][z] == nullptr || blocs[x][y][z - 1] == nullptr || this->world->getBlock(bpos) == Bloc::RES) {
				glPushMatrix();
				glTranslatef(x, y, z);
				blocs[x][y][z]->draw();
				glPopMatrix();
			}
		}
	}

	glEndList();
}

bool Chunk::isVisible(Vector3 bpos) {
	Vector3 pos = cpos * 16.0f + bpos;
	Vector3 toCheck[6] = { pos + Vector3(1,0,0), pos - Vector3(1,0,0), pos + Vector3(0,1,0), pos - Vector3(0,1,0),
						pos + Vector3(0,0,1), pos - Vector3(0,0,1) };
	for (int i = 0; i < 6; i++) {
		if (Block::isTransparent(getBlockWorld(toCheck[i]))) {
			return true;
		}
	}
	return false;
}

Bloc Chunk::getBlockWorld(Vector3 bpos) {
	if (bpos.x >= cpos.x * 16.0f && bpos.x < (cpos.x + 1) * 16.0f && bpos.y >= cpos.y * 16.0f &&
		bpos.y < (cpos.y + 1) * 16.0f && bpos.z >= cpos.z * 16.0f && bpos.z < (cpos.z + 1) * 16.0f) {
		if (this->blocs[(int)bpos.x % 16][(int)bpos.y % 16][(int)bpos.z % 16] == 0) {
			return Bloc::RES;
		}
		return this->blocs[(int)bpos.x % 16][(int)bpos.y % 16][(int)bpos.z % 16]->getId();
	}
	else {
		return this->world->getBlock(bpos);
	}
}