#include <GL/glew.h>
#include "Chunk.h"
#include "World.h"

Chunk::Chunk(World* world, Vector3 pos) {
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int z = 0; z < CHUNKSIZE; z++) {
				blocs[x][y][z] = nullptr;
			}
		}
	}
	this->world = world;
	this->cpos = pos;
	this->nblocs = 0;
	this->olist= glGenLists(1);
	this->tlist = glGenLists(1);

	this->firstdraw = false;
}

void Chunk::drawO() {
	if (nblocs == 0) {
		return;
	}
	if (firstdraw == false) {
		firstdraw = true;
		this->updateDL();
	}
	glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::BLOC));
	glCallList(olist);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Chunk::drawT() {
	if (nblocs == 0) {
		return;
	}
	if (firstdraw == false) {
		firstdraw = true;
		this->updateDL();
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glCallList(tlist);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	if (bloc->getId() != Bloc::RES) {
		this->blocs[(int)pos.x][(int)pos.y][(int)pos.z] = bloc;
		this->nblocs++;
	}

	return true;
}

Bloc Chunk::getBlock(Vector3 pos) {
	if (blocs[(int)pos.x][(int)pos.y][(int)pos.z] == 0) {
		return Bloc::RES;
	}
	return blocs[(int)pos.x][(int)pos.y][(int)pos.z]->getId();
}

void Chunk::update(int delta) {
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int z = 0; z < CHUNKSIZE; z++) {
				if (blocs[x][y][z] != 0) {
					blocs[x][y][z]->update(delta);
				}
			}
		}
	}
}

void Chunk::destroy() {
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int z = 0; z < CHUNKSIZE; z++) {
				if (blocs[x][y][z] != 0) {
					blocs[x][y][z]->destroy();
					delete blocs[x][y][z];
				}
			}
		}
	}
	glDeleteLists(olist, 1);
	glDeleteLists(tlist, 1);
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

void Chunk::updateDL() { //TODO: cas d'optimització world border, detectarà bloc res i se dibuixarà, no cal
	glDeleteLists(olist, 1);
	olist = glGenLists(1);
	glNewList(olist, GL_COMPILE);

	glTranslatef(0.5f, 0.5f, 0.5f);
	int nb = 0;
	for (int x = 0; x < CHUNKSIZE; x++) { //1a Passada: OPACS
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int y = 0; y < CHUNKSIZE; y++) {
				if (blocs[x][y][z] != 0) {
					Vector3 bpos = Vector3(x, y, z);
					//VERSIÓ ALTERNATIVA
					//Ordre: Esquerra, Damunt, Dreta, Abaix, Davant, Darrera
					Vector3 pos = cpos * CHUNKSIZE + bpos;
					Vector3 toCheck[6] = { pos - Vector3(1,0,0), pos + Vector3(0,1,0), pos + Vector3(1,0,0), pos - Vector3(0,1,0),
						pos + Vector3(0,0,1), pos - Vector3(0,0,1) };
					bool visible[6] = { false, false, false, false, false, false };
					bool qualcun = false;
					if (!Block::isTransparent(blocs[x][y][z]->getId())) {
						for (int i = 0; i < 6; i++) {
							if (Block::isTransparent(getBlockWorld(toCheck[i]))) {
								visible[i] = true;
								qualcun = true;
							}
						}
						nb++;
					}

					if (qualcun) {
						glPushMatrix();
						glTranslatef(x, y, z);
						world->br->drawBloc(blocs[x][y][z]->getId(), visible);
						glPopMatrix();
					}
					/*if (this->isVisible(Vector3(x,y,z))) {
						glPushMatrix();
						glTranslatef(x, y, z);
						blocs[x][y][z]->draw();
						glPopMatrix();
					}*/
					if (nb >= nblocs) { //No cal dibuixar més blocs
						y = CHUNKSIZE; z = CHUNKSIZE; x = CHUNKSIZE;
					}
				}
			}
		}
	}

	glEndList(); //FI llista opacs

	glDeleteLists(tlist, 1);
	tlist = glGenLists(1);
	glNewList(tlist, GL_COMPILE);

	glTranslatef(0.5f, 0.5f, 0.5f);
	for (int x = 0; x < CHUNKSIZE; x++) { //2a Passada: TRANSPARENTS
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int y = 0; y < CHUNKSIZE; y++) {
				if (blocs[x][y][z] != 0) {
					Vector3 bpos = Vector3(x, y, z);
					//VERSIÓ ALTERNATIVA
					//Ordre: Esquerra, Damunt, Dreta, Abaix, Davant, Darrera
					Vector3 pos = cpos * CHUNKSIZE + bpos;
					Vector3 toCheck[6] = { pos - Vector3(1,0,0), pos + Vector3(0,1,0), pos + Vector3(1,0,0), pos - Vector3(0,1,0),
						pos + Vector3(0,0,1), pos - Vector3(0,0,1) };
					bool visible[6] = { false, false, false, false, false, false };
					bool qualcun = false;
					if (Block::isTransparent(blocs[x][y][z]->getId())) { //Si és aigua, només s'ha de dibuixar si està en contacte amb aire o null
						Bloc btipus = blocs[x][y][z]->getId();
						for (int i = 0; i < 6; i++) {
							Bloc bl = getBlockWorld(toCheck[i]);
							if (Block::isTransparent(bl) && btipus != bl) {
								visible[i] = true;
								qualcun = true;
							}
						}
						nb++;
					}

					if (qualcun) {
						glPushMatrix();
						glTranslatef(x, y, z);
						world->br->drawBloc(blocs[x][y][z]->getId(), visible);
						glPopMatrix();
					}

					if (nb >= nblocs) { //No cal dibuixar més blocs
						y = CHUNKSIZE; z = CHUNKSIZE; x = CHUNKSIZE;
					}
					/*if (this->isVisible(Vector3(x,y,z))) {
						glPushMatrix();
						glTranslatef(x, y, z);
						blocs[x][y][z]->draw();
						glPopMatrix();
					}*/
				}
			}
		}
	}
	glEndList(); //Fi llista transparent
}

bool Chunk::isVisible(Vector3 bpos) {
	Vector3 pos = cpos * CHUNKSIZE + bpos;
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
	if (bpos.x >= world->sizex * CHUNKSIZE || bpos.y >= world->sizey * CHUNKSIZE || bpos.z >= world->sizez * CHUNKSIZE ||
		bpos.x < 0 || bpos.y < 0 || bpos.z < 0) {
		return Bloc::TERRA; //Optimització no dibuixar bloc border
	}
	if (bpos.x >= cpos.x * CHUNKSIZE && bpos.x < (cpos.x + 1) * CHUNKSIZE && bpos.y >= cpos.y * CHUNKSIZE &&
		bpos.y < (cpos.y + 1) * CHUNKSIZE && bpos.z >= cpos.z * CHUNKSIZE && bpos.z < (cpos.z + 1) * CHUNKSIZE) {
		if (this->blocs[(int)bpos.x % CHUNKSIZE][(int)bpos.y % CHUNKSIZE][(int)bpos.z % CHUNKSIZE] == 0) {
			return Bloc::RES;
		}
		return this->blocs[(int)bpos.x % CHUNKSIZE][(int)bpos.y % CHUNKSIZE][(int)bpos.z % CHUNKSIZE]->getId();
	}
	else {
		return this->world->getBlock(bpos);
	}
}

bool Chunk::getByteData(char* arr) {
	int desp = 0;
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int z = 0; z < CHUNKSIZE; z++) {
				arr[desp++] = static_cast<unsigned char>(this->getBlock(Vector3((float)x, (float)y, (float)z)));
			}
		}
	}
	return true;
}

bool Chunk::readFromByteData(char* arr) {
	int desp = 0;
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int z = 0; z < CHUNKSIZE; z++) {
				//printf("%d ", arr[desp]);
				Bloc tipus = static_cast<Bloc>(arr[desp++]);
				world->setBlock(tipus, (this->cpos * CHUNKSIZE) + Vector3(x, y, z), nullptr, false);
			}
		}
	}
	return true;
}