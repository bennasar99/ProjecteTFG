#pragma once
#include "../TextureManager.h"
#include "../ModelManager.h"
#include "../TextureAtlas.h"
#include "Block.h"

class BlockRenderer {
	TextureAtlas *blockAtlas;
	void drawCub(bool cares[6], int texNum);
	void drawCub(bool cares[6]);
public:
	void drawBloc(Bloc id);
	void drawBloc(Bloc id, bool cares[6]);
	float* getTexCoords(Bloc tipus);
	float* getColor(Bloc tipus);

	BlockRenderer();
};