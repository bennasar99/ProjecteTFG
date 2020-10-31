#pragma once
#include <list>
#include "Utils.h"

#define NTEXT 7 //Nombre de textures

enum class Textura {
	TERRA,
	FUSTA,
	PEDRA,
	ALTAVEU,
	ESTALAGMITA,
	FUSTAARBRE,
	FULLAARBRE,
	BLOC
};

enum class Cara {
	ABAIX,
	ADALT,
	DAVANT,
	DARRERA,
	DRETA,
	ESQUERRA
};

/*
	S'encarrega de gestionar les textures
*/
class TextureManager {
private:
	static unsigned int text[NTEXT]; //Textures

	TextureManager() {}
public:
	static bool LoadTexture(const char* path, Textura nom);
	static unsigned int getTexture(Textura nom);
};