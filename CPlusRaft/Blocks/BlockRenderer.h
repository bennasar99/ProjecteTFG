#pragma once
#include "../Utils/TextureManager.h"
#include "../Utils/TextureAtlas.h"
#include "Block.h"

class BlockRenderer {
	TextureAtlas *blockAtlas;
	void drawCub(bool cares[6], std::array<float, 4> texCoords);
public:
	void drawBloc(Bloc id);
	void drawBloc(Bloc id, bool cares[6]);

	BlockRenderer();
};