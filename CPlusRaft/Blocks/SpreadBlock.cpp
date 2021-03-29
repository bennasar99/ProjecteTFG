#include "SpreadBlock.h"

#include <iostream>
#include <cmath>
#include "../World.h"


SpreadBlock::SpreadBlock(Bloc id, Vector3<int> pos) : Block(id) {
	this->pos = pos;
	this->growTimer = 1000;
}

void SpreadBlock::destroy(World* world) {

}

void SpreadBlock::update(float delta, World* world) {
	//if (world->getBlock(this->pos + Vector3<int>(0, -1, 0)) == Bloc::RES) {
	//	world->deleteBlock(this->pos, true);
	//	return;
	//}
	//this->growTimer-=delta;
	////printf("grow %d\n", growTimer);
	//if (growTimer <= 0) {
	//	if (rand() % 3 == 2) {
	//		if (world->getBlock(this->pos + Vector3<int>(0, 1, 0)) == Bloc::AIRE || world->getBlock(this->pos + Vector3<int>(0, 1, 0)) == Bloc::RES) {
	//			world->setBlock(Bloc::HERBAFULL, this->pos + Vector3<int>(0, 1, 0));
	//		}
	//	}
	//	growTimer = 1000;
	//}
}

void SpreadBlock::interact(World* world) {

}

//Funció de dibuixat (Del bloc/objecte tal com és, no icona)
void SpreadBlock::draw(ChunkMesh* cM, bool visible[6], Vector3<int> relPos) {

	//GLfloat vertP[17][2][3] = {
	//{ {0.5, 0.3, 0.5}, {0.5, -0.5, 0.5} },
	//{ {-0.5, 0.5, 0.5}, {-0.5, -0.5, 0.5}},
	//{ {0.5, 0.1, -0.5}, {0.5, -0.5, -0.5} },
	//{ {-0.5, 0.4, -0.5}, { -0.5, -0.5, -0.5}},
	//{ {0.0, 0.5, 0.0}, { 0.0, -0.5, 0.0}},
	//{ {0.5, 0.2, 0.0}, { 0.5, -0.5, 0.0}},
	//{ {0.0, 0.4, 0.5}, { 0.0, -0.5, 0.5}},
	//{ {-0.5, 0.2, 0.0}, { -0.5, -0.5, 0.0}},
	//{ {0.0, 0.4, -0.5}, { 0.0, -0.5, -0.5}},
	//{ {0.25, 0.3, 0.25}, { 0.25, -0.5, 0.25}},
	//{ {-0.25, 0.5, 0.25}, { -0.25, -0.5, 0.25}},
	//{ {0.25, 0.1, -0.25}, { 0.25, -0.5, -0.25}},
	//{ {-0.25, 0.4, -0.25}, { -0.25, -0.5, -0.25}},
	//{ {0.25, 0.3, 0}, { 0.25, -0.5, 0}},
	//{ {0, 0.5, 0.25}, { 0, -0.5, 0.25}},
	//{ {-0.25, 0.1, 0}, { -0.25, -0.5, 0}},
	//{ {0, 0.4, -0.25}, { 0, -0.5, -0.25}}
	//};

	GLfloat vert[17][2][3] = {
	{ {0.5, 0.5, 0.5}, {0.5, -0.5, 0.5} },
	{ {-0.5, 0.5, 0.5}, {-0.5, -0.5, 0.5}},
	{ {0.5, 0.5, -0.5}, {0.5, -0.5, -0.5} },
	{ {-0.5, 0.5, -0.5}, { -0.5, -0.5, -0.5}},
	{ {0.0, 0.5, 0.0}, { 0.0, -0.5, 0.0}},
	{ {0.5, 0.5, 0.0}, { 0.5, -0.5, 0.0}},
	{ {0.0, 0.5, 0.5}, { 0.0, -0.5, 0.5}},
	{ {-0.5, 0.5, 0.0}, { -0.5, -0.5, 0.0}},
	{ {0.0, 0.5, -0.5}, { 0.0, -0.5, -0.5}},
	{ {0.25, 0.5, 0.25}, { 0.25, -0.5, 0.25}},
	{ {-0.25, 0.5, 0.25}, { -0.25, -0.5, 0.25}},
	{ {0.25, 0.5, -0.25}, { 0.25, -0.5, -0.25}},
	{ {-0.25, 0.5, -0.25}, { -0.25, -0.5, -0.25}},
	{ {0.25, 0.5, 0}, { 0.25, -0.5, 0}},
	{ {0, 0.5, 0.25}, { 0, -0.5, 0.25}},
	{ {-0.25, 0.5, 0}, { -0.25, -0.5, 0}},
	{ {0, 0.5, -0.25}, { 0, -0.5, -0.25}}
	};

    int texNum = -1; //Per defecte sense textura
    unsigned char color[4] = { 0, 255, 0, 255 }; //Verd

	//float* texCoords = TextureManager::getTexCoords(texNum);
	float xb = 0, yb = 0, xt = 0, yt = 0;
	//xb = texCoords[0]; yb = texCoords[1]; xt = texCoords[2]; yt = texCoords[3];

	for (int i = 0; i < 17; i++) {
		for (int j = 0; j < 2; j++) {
			unsigned short normal[3] = { 0,1,0 };
			unsigned short text[2] = { 0,0 };
			unsigned short vPos[3] = { vert[i][j][0], vert[i][j][1], vert[i][j][2] };
			vPos[0] += relPos.x; vPos[1] += relPos.y; vPos[2] += relPos.z;
			cM->addVertexO(vPos, normal, color, text, Primitiva::LINIA);
		}
	}

}