#include "Block.h"
#include "../World.h"
#include <iostream>

TextureAtlas blockAtlas = TextureAtlas(1, 7);

Block::Block(World* world, Bloc id, Block* parent) {
	this->world = world;
	this->id = id;
	this->parent = parent;
}

Block::Block() {
	this->world = NULL;
	this->id = Bloc::RES;
	this->parent = 0;
}

void Block::update(int delta) {

}

Block* Block::getParent() {
	return this->parent;
}

Bloc Block::getId() {
	return this->id;
}

void Block::setId(Bloc id) {
	this->id = id;
}

void Block::destroy(){
	if (parent != 0) {
		this->parent->destroy();
	}
};

void Block::interact() {
	if (parent != 0) {
		this->parent->interact();
	}
};

void Block::draw(ChunkMesh* cM, bool visible[6], Vector3 relPos) {
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

	int texNum = -1; //Per defecte sense textura
	float color[4] = { 0, 0, 0, 1 }; //RGBA, Abstracció classe Color?

	switch (this->id) {
	case Bloc::CUB: //Cub vermell
		color[0] = 1; color[1] = 0; color[2] = 0; color[3] = 1;
		break;
	case Bloc::TERRA: //Cub marró
		texNum = 6;
		color[0] = 0.5f; color[1] = 0.35f; color[2] = 0.05f; color[3] = 1;
		break;
	case Bloc::VIDRE: //cub transparent/opac (vidre?)
		color[0] = 0; color[1] = 0; color[2] = 0; color[3] = 1;
		break;
	case Bloc::FUSTA:
		texNum = 3;
		color[0] = 0.76f; color[1] = 0.6f; color[2] = 0.42f; color[3] = 1;
		break;
	case Bloc::PEDRA:
		texNum = 5;
		color[0] = 0.5f; color[1] = 0.5f; color[2] = 0.5f; color[3] = 1;
		break;
	case Bloc::FUSTAARBRE:
		texNum = 4;
		color[0] = 0.76f; color[1] = 0.6f; color[2] = 0.42f; color[3] = 1;
		break;
	case Bloc::FULLAARBRE:
		texNum = 2;
		color[0] = 0; color[1] = 0.5f; color[2] = 0; color[3] = 1;
		break;
	}

	float* texCoords = TextureManager::getTexCoords(texNum);
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

	for (int i = 0; i < 6; i++) {
		if (visible[i]) {
			for (int j = 0; j < 4; j++) {
				float vPos[3] = { vert[i][j][0], vert[i][j][1], vert[i][j][2] };
				vPos[0] += relPos.x; vPos[1] += relPos.y; vPos[2] += relPos.z;
				cM->addVertexO(vPos, normals[i][j], color, text[i][j], Primitiva::QUAD);
			}
		}
	}
}

bool Block::isTransparent(Bloc tipus) {
	if (tipus == Bloc::RES || tipus == Bloc::AIRE || tipus == Bloc::HERBA || tipus == Bloc::VIDRE || tipus == Bloc::TORXA
		|| tipus == Bloc::HERBAFULL || tipus == Bloc::AIGUA) {
		return true;
	}
	return false;
}

bool Block::isSolid(Bloc tipus) {
	if (tipus != Bloc::RES && tipus != Bloc::AIRE && tipus != Bloc::HERBA && tipus != Bloc::TORXA && tipus != Bloc::HERBAFULL 
		&& tipus != Bloc::AIGUA) { //Exclude list
		return true;
	}
	return false;
}