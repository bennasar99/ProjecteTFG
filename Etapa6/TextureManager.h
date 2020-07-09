#pragma once
#include <list>
#include "Utils.h"

#define NTEXT 5 //Nombre de textures

enum class Textura {
	TERRA,
	FUSTA,
	PEDRA,
	ALTAVEU,
	ESTALAGMITA
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