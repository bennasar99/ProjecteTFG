#pragma once
#include <list>
#include "../Utils.h"
#include "TextureAtlas.h"

#define NTEXT 7 //Nombre de textures

enum class Textura {
	OVELLA,
	BLOC,
	ESTRUC,
	FONT
};

/*
	S'encarrega de gestionar les textures
*/
class TextureManager {
private:
	static unsigned int text[NTEXT]; //Textures
	static TextureAtlas blockAtlas;
	static TextureAtlas fontAtlas;

	TextureManager() {}
public:
	static bool LoadTexture(const char* path, Textura nom);
	static unsigned int getTexture(Textura nom);
	static void initialize();
	static bool getTexCoords(int texNum, std::array<float, 4> &texCoords);
	static bool getFontTexCoords(char c, std::array<float, 4>& fontTexCoords);
};