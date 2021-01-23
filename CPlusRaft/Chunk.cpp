#include "Chunk.h"
#include "World.h"

Chunk::Chunk(World* world, Vector3<int> pos) {
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

	this->cMesh = new ChunkMesh();
}

void Chunk::drawO() {
	if (nblocs == 0) {
		return;
	}
	if (firstdraw == true) {
		firstdraw = false;
		cMesh->update();
	}
	glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::BLOC));
	glTranslatef(0.5f, 0.5f, 0.5f);
	glFrontFace(GL_CCW);
	this->cMesh->drawO();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Chunk::drawT() {
	if (nblocs == 0) {
		return;
	}
	if (firstdraw == true) {
		firstdraw = false;
		cMesh->update();
	}
	glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::BLOC));
	glFrontFace(GL_CCW);
	glTranslatef(0.5f, 0.5f, 0.5f);
	this->cMesh->drawT();
	glBindTexture(GL_TEXTURE_2D, 0);
}

Vector3<int> Chunk::getPos() {
	return cpos;
}

bool Chunk::setBlock(Block* bloc, Vector3<int> pos) {
	//Hi ha blocs amb les seves pròpies classes, sinó s'utilitza la classe genèrica
	if (this->blocs[pos.x][pos.y][pos.z] != nullptr) {
		this->blocs[pos.x][pos.y][pos.z]->destroy(this->world);
		delete this->blocs[pos.x][pos.y][pos.z];
		this->nblocs--;
	}
	if (bloc->getId() != Bloc::RES) {
		this->blocs[pos.x][pos.y][pos.z] = bloc;
		this->nblocs++;
	}

	return true;
}

Bloc Chunk::getBlock(Vector3<int> pos) {
	if (blocs[pos.x][pos.y][pos.z] == nullptr) {
		return Bloc::RES;
	}
	return blocs[pos.x][pos.y][pos.z]->getId();
}

void Chunk::update(int delta) {
	for (int x = 0; x < CHUNKSIZE; x++) {
			for (int z = 0; z < CHUNKSIZE; z++) {
				if (blocs[x][lastYupd][z] != 0) {
					blocs[x][lastYupd][z]->update(delta, this->world);
				}
			}
	}
	lastYupd++;
	lastYupd %= CHUNKSIZE;
}

void Chunk::destroy() {
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int z = 0; z < CHUNKSIZE; z++) {
				if (blocs[x][y][z] != 0) {
					blocs[x][y][z]->destroy(this->world);
					delete blocs[x][y][z];
				}
			}
		}
	}
	cMesh->erase();
	delete cMesh;
}

//TODO actualitzar display list
bool Chunk::delBlock(Vector3<int> bpos, bool destroy) {
	printf("%d %d %d\n", bpos.x, bpos.y, bpos.z);
  	if (this->blocs[bpos.x][bpos.y][bpos.z] != nullptr) {      
		if (destroy) {
			this->blocs[bpos.x][bpos.y][bpos.z]->destroy(this->world);
		}
		delete this->blocs[bpos.x][bpos.y][bpos.z];
		this->blocs[bpos.x][bpos.y][bpos.z] = 0;
		nblocs--;
		this->updateMesh();
		world->updateNeighborChunks(this->cpos, bpos);
	}
	return true;
}

void Chunk::interact(Vector3<int> bpos) {
	if (blocs[bpos.x][bpos.y][bpos.z] == 0) {
		return;
	}
	blocs[bpos.x][bpos.y][bpos.z]->interact(this->world);
}

bool Chunk::isVisible(Vector3<int> bpos) {
	Vector3<int> pos = cpos * CHUNKSIZE + bpos;
	Vector3<int> toCheck[6] = { pos + Vector3<int>(1,0,0), pos - Vector3<int>(1,0,0), pos + Vector3<int>(0,1,0), pos - Vector3<int>(0,1,0),
						pos + Vector3<int>(0,0,1), pos - Vector3<int>(0,0,1) };
	for (int i = 0; i < 6; i++) {
		if (Block::isTransparent(getBlockWorld(toCheck[i]))) {
			return true;
		}
	}
	
	return false;
}

Bloc Chunk::getBlockWorld(Vector3<int> bpos) {
	if (bpos.x >= world->size.x * CHUNKSIZE || bpos.y >= world->size.y * CHUNKSIZE || bpos.z >= world->size.z * CHUNKSIZE ||
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

bool Chunk::setBlockWorld(Block* bloc, Vector3<int> bpos) {
	/*if (bpos.x >= world->size.x * CHUNKSIZE || bpos.y >= world->size.y * CHUNKSIZE || bpos.z >= world->size.z * CHUNKSIZE ||
		bpos.x < 0 || bpos.y < 0 || bpos.z < 0) {
		return false;
	}*/
	if (bpos.x >= cpos.x * CHUNKSIZE && bpos.x < (cpos.x + 1) * CHUNKSIZE && bpos.y >= cpos.y * CHUNKSIZE &&
		bpos.y < (cpos.y + 1) * CHUNKSIZE && bpos.z >= cpos.z * CHUNKSIZE && bpos.z < (cpos.z + 1) * CHUNKSIZE) {
		return this->setBlock(bloc, bpos % 16);
	}
	else {
		return this->world->setBlock(bloc, bpos, false);
	}
}

bool Chunk::getByteData(char* arr) {
	int desp = 0;
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int z = 0; z < CHUNKSIZE; z++) {
				arr[desp++] = static_cast<unsigned char>(this->getBlock(Vector3<int>(x, y, z)));
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
				if (tipus != Bloc::RES) {
					world->setBlock(tipus, (this->cpos * CHUNKSIZE) + Vector3<int>(x, y, z), nullptr, false);
				}
			}
		}
	}
	this->updateMesh();
	return true;
}

void Chunk::updateMesh() {
	cMesh->erase();

	int nb = 0;
	for (int x = 0; x < CHUNKSIZE; x++) { //1a Passada: OPACS
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int y = 0; y < CHUNKSIZE; y++) {
				if (blocs[x][y][z] != 0) {
					Vector3 bpos = Vector3<int>(x, y, z);
					//Ordre: Esquerra, Damunt, Dreta, Abaix, Davant, Darrera
					Vector3<int> pos = cpos * CHUNKSIZE + bpos;
					Vector3<int> toCheck[6] = { pos - Vector3<int>(1,0,0), pos + Vector3<int>(0,1,0), pos + Vector3<int>(1,0,0), pos - Vector3<int>(0,1,0),
						pos + Vector3<int>(0,0,1), pos - Vector3<int>(0,0,1) };

					bool qualcun = false;
					bool visible[6] = { false, false, false, false, false, false };
					if (Block::isTransparent(blocs[x][y][z]->getId())) {
						for (int i = 0; i < 6; i++) {
							if (Block::isTransparent(getBlockWorld(toCheck[i])) && getBlockWorld(toCheck[i]) != blocs[x][y][z]->getId()) {
								visible[i] = true;
								qualcun = true;
							}
						}
					} else {
						for (int i = 0; i < 6; i++) {
							if (Block::isTransparent(getBlockWorld(toCheck[i]))) {
								visible[i] = true;
								qualcun = true;
							}
						}
					}
					if (qualcun) {
						blocs[x][y][z]->draw(cMesh, visible, Vector3<int>(x, y, z));
					}
					nb++;

					if (nb >= nblocs) { //No cal dibuixar més blocs
						y = CHUNKSIZE; z = CHUNKSIZE; x = CHUNKSIZE;
					}
				}
			}
		}
	}
	this->firstdraw = true;
}

Bioma Chunk::getBiome() {
	return this->bio;
}

void Chunk::setBiome(Bioma biome) {
	this->bio = biome;
}