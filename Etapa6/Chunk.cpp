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

	this->cMesh = new ChunkMesh();

	this->firstdraw = false;
}

void Chunk::drawO() {
	if (nblocs == 0) {
		return;
	}
	if (firstdraw == false) {
		firstdraw = true;
		this->updateMesh();
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
	if (firstdraw == false) {
		firstdraw = true;
		this->updateMesh();
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glFrontFace(GL_CCW);
	glTranslatef(0.5f, 0.5f, 0.5f);
	this->cMesh->drawT();
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
	cMesh->erase();
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
		//this->updateDL();
		this->updateMesh();
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

void Chunk::updateMesh() {
	cMesh->erase();

	GLfloat vert[6][4][3] = {
		{{-.5f, .5f, .5f},  {-.5f, .5f,-.5f},  {-.5f,-.5f,-.5f}, {-.5f,-.5f, .5f}}, // v1,v6,v7,v2 (left)
		{{.5f, .5f, .5f},   {.5f, .5f,-.5f},  {-.5f, .5f,-.5f}, {-.5f, .5f, .5f}}, // v0,v5,v6,v1 (top)
		{{.5f, .5f, .5f},   {.5f,-.5f, .5f},   {.5f,-.5f,-.5f},  {.5f, .5f,-.5f}}, // v0,v3,v4,v5 (right)
		{{-.5f,-.5f,-.5f},   {.5f,-.5f,-.5f},   {.5f,-.5f, .5f}, {-.5f,-.5f, .5f}}, // v7,v4,v3,v2 (bottom)
		{{.5f, .5f, .5f},  {-.5f, .5f, .5f},  {-.5f,-.5f, .5f},  {.5f,-.5f, .5f}}, // v0,v1,v2,v3 (front)
		{{.5f,-.5f,-.5f},  {-.5f,-.5f,-.5f},  {-.5f, .5f,-.5f},  {.5f, .5f,-.5f}}  // v4,v7,v6,v5 (back)
	};

	// normal array
	GLfloat normals[6][4][3] = {
		{{-1, 0, 0},  {-1, 0, 0},  {-1, 0, 0},  {-1, 0, 0}},  // v1,v6,v7,v2 (left)
		{{0, 1, 0},   {0, 1, 0},   {0, 1, 0},   {0, 1, 0}},  // v0,v5,v6,v1 (top)
		{{1, 0, 0},   {1, 0, 0},   {1, 0, 0},   {1, 0, 0}},  // v0,v3,v4,v5 (right)
		{{0,-1, 0},   {0,-1, 0},   {0,-1, 0},   {0,-1, 0}},  // v7,v4,v3,v2 (bottom)
		{{0, 0, 1},   {0, 0, 1},   {0, 0, 1},   {0, 0, 1}},  // v0,v1,v2,v3 (front)
		{{0, 0,-1},   {0, 0,-1},  {0, 0,-1},   {0, 0,-1}}   // v4,v7,v6,v5 (back)
	};

	// texture coord array
	GLfloat texCoords[6][4][2] = {
		{{1, 0},   {0, 0},   {0, 1},   {1, 1}},               // v1,v6,v7,v2 (left)
		{{1, 1},   {1, 0},   {0, 0},   {0, 1}},               // v0,v5,v6,v1 (top)
		{{0, 0},   {0, 1},   {1, 1},   {1, 0}},               // v0,v3,v4,v5 (right)
		{{0, 1},   {1, 1},   {1, 0},   {0, 0}},               // v7,v4,v3,v2 (bottom)
		{{1, 0},   {0, 0},   {0, 1},   {1, 1}},               // v0,v1,v2,v3 (front)
		{{0, 1},   {1, 1},   {1, 0},   {0, 0}}                // v4,v7,v6,v5 (back)
	};

	GLuint indices[6][2][3] = {
		{ {12,13,14},  {14,15,12} },    // v1-v6-v7, v7-v2-v1 (left)
		{ {8, 9,10},  {10,11, 8} },    // v0-v5-v6, v6-v1-v0 (top)
		{ {4, 5, 6},   {6, 7, 4} },    // v0-v3-v4, v4-v5-v0 (right)
		{ {16,17,18},  {18,19,16} },    // v7-v4-v3, v3-v2-v7 (bottom)
		{ {0, 1, 2},   {2, 3, 0} },    // v0-v1-v2, v2-v3-v0 (front)
		{ {20,21,22},  {22,23,20} }     // v4-v7-v6, v6-v5-v4 (back)
	};

	int nb = 0;
	for (int x = 0; x < CHUNKSIZE; x++) { //1a Passada: OPACS
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int y = 0; y < CHUNKSIZE; y++) {
				if (blocs[x][y][z] != 0) {
					Vector3 bpos = Vector3(x, y, z);
					//Ordre: Esquerra, Damunt, Dreta, Abaix, Davant, Darrera
					Vector3 pos = cpos * CHUNKSIZE + bpos;
					Vector3 toCheck[6] = { pos - Vector3(1,0,0), pos + Vector3(0,1,0), pos + Vector3(1,0,0), pos - Vector3(0,1,0),
						pos + Vector3(0,0,1), pos - Vector3(0,0,1) };

					float* texCoords = this->world->br->getTexCoords(blocs[x][y][z]->getId());
					float* color = this->world->br->getColor(blocs[x][y][z]->getId());
					float xb = 0, yb = 0, xt = 0, yt = 0;
					xb = texCoords[0]; yb = texCoords[1]; xt = texCoords[2]; yt = texCoords[3];

					GLfloat text[6][4][2] =
					{
						{{-xt,yt}, {xb,yt}, {xb,yb}, {-xt, yb}}, //Esquerra OK
						{{-xt,yb}, {-xt,yt}, {xb,yt}, {xb,yb}}, //Damunt OK
						{{xt, yb}, {xb,yb}, {xb,yt}, {xt,yt}}, //Dreta OK
						{{xt,yt}, {xt,yb}, {xb,yb}, {xb,yt}}, //Abaix OK
						{{xt, yt}, {xt,yb}, {xb,yb}, {xb,yt}}, //Davant OK
						{{-xt,yb}, {-xt,yt}, {xb,yt}, {xb,yb}} //Darrera OK
					};

					if (Block::isTransparent(blocs[x][y][z]->getId())) {
						for (int i = 0; i < 6; i++) {
							if (Block::isTransparent(getBlockWorld(toCheck[i])) && getBlockWorld(toCheck[i]) != blocs[x][y][z]->getId()) {
								for (int j = 0; j < 4; j++) {
									float vPos[3] = { vert[i][j][0], vert[i][j][1], vert[i][j][2] };
									vPos[0] += x; vPos[1] += y; vPos[2] += z;
									cMesh->addVertexT(vPos, normals[i][j], color, text[i][j]);
								}
							}
						}
					} else {
						for (int i = 0; i < 6; i++) {
							if (Block::isTransparent(getBlockWorld(toCheck[i]))) {
								for (int j = 0; j < 4; j++) {
									float vPos[3] = { vert[i][j][0], vert[i][j][1], vert[i][j][2] };
									vPos[0] += x; vPos[1] += y; vPos[2] += z;
									cMesh->addVertexO(vPos, normals[i][j], color, text[i][j], Primitiva::QUAD);
								}
							}
						}
					}
					nb++;

					if (nb >= nblocs) { //No cal dibuixar més blocs
						y = CHUNKSIZE; z = CHUNKSIZE; x = CHUNKSIZE;
					}
				}
			}
		}
	}
	cMesh->update();
}