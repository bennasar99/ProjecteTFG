#include "SolidBlock.h"

#include <iostream>
#include <cmath>
#include "../World.h"


SolidBlock::SolidBlock(Bloc id) : Block(id) {

}

//Funció de dibuixat (Del bloc/objecte tal com és, no icona)
void SolidBlock::draw(ChunkMesh* cM, bool visible[6], Vector3<int> relPos) {
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
		case Bloc::NEU:
			texNum = 1;
			color[0] = 1; color[1] = 1; color[2] = 1; color[3] = 1;
			break;
		case Bloc::GEL:
			texNum = 7;
			color[0] = 0; color[1] = 0; color[2] = 1; color[3] = 0.8f;
			break;
		case Bloc::ARENA:
			color[0] = 0.929f; color[1] = 0.788f; color[2] = 0.686f; color[3] = 1;
			texNum = 18;
			break;
		case Bloc::OR:
			color[0] = 1; color[1] = 0.843f; color[2] = 0; color[3] = 1;
			texNum = 12;
			break;
		case Bloc::FANG:
			color[0] = 0.5f; color[1] = 0.35f; color[2] = 0.05f; color[3] = 1;
			texNum = 8;
			break;
		case Bloc::FERRO:
			color[0] = 0.5f; color[1] = 0.5f; color[2] = 0.5f; color[3] = 1;
			texNum = 9;
			break;
		case Bloc::GRAVILLA:
			color[0] = 0.5f; color[1] = 0.5f; color[2] = 0.5f; color[3] = 1;
			texNum = 14;
			break;
		case Bloc::CARBO:
			color[0] = 0.21f; color[1] = 0.271f; color[2] = 0.31f; color[3] = 1;
			texNum = 10;
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
				if (Block::isTransparent(this->id)) {
					cM->addVertexT(vPos, normals[i][j], color, text[i][j]);
				}
				else {
					cM->addVertexO(vPos, normals[i][j], color, text[i][j], Primitiva::QUAD);
				}
			}
		}
	}

}