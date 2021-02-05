#include "LiquidBlock.h"

#include <iostream>
#include <cmath>
#include "../World.h"


LiquidBlock::LiquidBlock(Bloc id, Vector3<int> pos) : Block(id) {
	this->lvl = 16;
	this->pos = pos;
}

LiquidBlock::LiquidBlock(Bloc id, Vector3<int> pos, int lvl) : Block(id) {
	this->lvl = lvl;
	this->pos = pos;
}

void LiquidBlock::destroy(World* world) {

}

void LiquidBlock::interact(World* world) {

}

void LiquidBlock::update(float delta, World* world) {
	//Vector3<int> toCheck[4] = { pos + Vector3<int>(1,0,0), pos + Vector3<int>(-1,0,0) , pos + Vector3<int>(0,0,1), pos + Vector3<int>(0,0,-1) };
	//for (int i = 0; i < 4; i++) {
	//	if ((toCheck[i].x > 0) && (toCheck[i].x < (world->sizex * CHUNKSIZE - 1)) && 
	//		(toCheck[i].z > 0) && (toCheck[i].z < (world->sizez * CHUNKSIZE - 1))) {
	//		//printf("block %d %d %d: %d\n", toCheck[i].x, toCheck[i].y, toCheck[i].z, world->getBlock(toCheck[i]));
	//		if ((world->getBlock(toCheck[i]) == Bloc::RES) || (world->getBlock(toCheck[i]) == Bloc::AIRE)) {
	//			Block* newb = new LiquidBlock(this->id, toCheck[i], 5);
	//			world->setBlock(newb, toCheck[i], true);
	//			this->lvl--;
	//		}
	//	}
	//}
}

void LiquidBlock::setLvl(int lvl) {
	this->lvl = lvl;
}

//Funció de dibuixat (Del bloc/objecte tal com és, no icona)
void LiquidBlock::draw(ChunkMesh* cM, bool visible[6], Vector3<int> relPos) {
	float yt = -0.5f + 1.0f * (this->lvl / 16);
    GLfloat vert[6][4][3] = {
    {{-.5f, yt, .5f},  {-.5f, yt,-.5f},  {-.5f,-.5f,-.5f}, {-.5f,-.5f, .5f}}, // v1,v6,v7,v2 (left)
    {{.5f, yt, .5f},   {.5f, yt,-.5f},  {-.5f, yt,-.5f}, {-.5f, yt, .5f}}, // v0,v5,v6,v1 (top)
    {{.5f, yt, .5f},   {.5f,-.5f, .5f},   {.5f,-.5f,-.5f},  {.5f, yt,-.5f}}, // v0,v3,v4,v5 (right)
    {{-.5f,-.5f,-.5f},   {.5f,-.5f,-.5f},   {.5f,-.5f, .5f}, {-.5f,-.5f, .5f}}, // v7,v4,v3,v2 (bottom)
    {{.5f, yt, .5f},  {-.5f, yt, .5f},  {-.5f,-.5f, .5f},  {.5f,-.5f, .5f}}, // v0,v1,v2,v3 (front)
    {{.5f,-.5f,-.5f},  {-.5f,-.5f,-.5f},  {-.5f, yt,-.5f},  {.5f, yt,-.5f}}  // v4,v7,v6,v5 (back)
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
		case Bloc::AIGUA: //AIGUA
			color[0] = 0; color[1] = 0; color[2] = 1; color[3] = 0.7f;
			break;
	}

	//float* texCoords = TextureManager::getTexCoords(texNum);
	float xb = 0, yb = 0, xt = 0;
	yt = 0;
	//xb = texCoords[0]; yb = texCoords[1]; xt = texCoords[2]; yt = texCoords[3];

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
			//Doble dibuixat, com que podem entrar dins un liquid, volem que des de dins també es vegi el liquid
			//(ex: davall aigua) tot i tenir cull face activat.
			for (int j = 0; j < 4; j++) {
				float vPos[3] = { vert[i][j][0], vert[i][j][1], vert[i][j][2] };
				vPos[0] += relPos.x; vPos[1] += relPos.y; vPos[2] += relPos.z;
				cM->addVertexT(vPos, normals[i][j], color, text[i][j]);
			}
			for (int j = 3; j >= 0; j--) {
				float vPos[3] = { vert[i][j][0], vert[i][j][1], vert[i][j][2] };
				vPos[0] += relPos.x; vPos[1] += relPos.y; vPos[2] += relPos.z;
				cM->addVertexT(vPos, normals[i][j], color, text[i][j]);
			}
		}
	}

}