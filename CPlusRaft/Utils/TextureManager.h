#pragma once
#include <list>
#include "../Utils.h"
#include "TextureAtlas.h"
#include <unordered_map>

using namespace std;

/*
	S'encarrega de gestionar les textures
*/
class TextureManager {
private:
	//static unsigned int text[NTEXT]; //Textures
	static unordered_map<string, unsigned int> text;
	static TextureAtlas blockAtlas;
	static TextureAtlas fontAtlas;

	TextureManager() {}
public:
	static bool LoadTexture(const char* path, string nom);
	static unsigned int getTexture(string nom);
	static void applyTexture(string nom);
	static void noTexture();
	static void initialize();
	static bool getBlockTexCoords(int texNum, std::array<float, 4> &texCoords);
	static bool getFontTexCoords(char c, std::array<float, 4>& fontTexCoords);
};