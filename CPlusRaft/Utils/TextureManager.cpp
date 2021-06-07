#include "GL/glew.h"
#include "TextureManager.h"

unordered_map<string, unsigned int> TextureManager::text;
TextureAtlas TextureManager::blockAtlas;
TextureAtlas TextureManager::fontAtlas;

//Carrega una textura, assignant-la a una "Textura" (enum) concreta
bool TextureManager::LoadTexture(const char* path, string nom) {

	glGenTextures(1, &TextureManager::text[nom]); //Generam la textura
	glBindTexture(GL_TEXTURE_2D, TextureManager::text[nom]); //Seleccionam la textura

	//Característiques de la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_NEAREST



	//Carregam les dades de la imatge que emprarem com a textura
	int width, height, canals;
	unsigned char* data = stbi_load(path, &width, &height, &canals, STBI_rgb_alpha);

	if (data) { //La càrrega és correcte, assignam la imatge a la textura
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); //Generam el mipmap de la textura
	}
	else {
		printf("TEXTURA %s FALLA!\n", path);
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, 0); //Deseleccionam la textura
	stbi_image_free(data); //Alliberam les dades de l'imatge

	return true;
}

//Retornam la textura corresponent a "nom"
unsigned int TextureManager::getTexture(string nom) {
	return TextureManager::text[nom];
}

//Aplica la textura corresponent a "nom"
void TextureManager::applyTexture(string nom) {
	glBindTexture(GL_TEXTURE_2D, TextureManager::text[nom]);
}

//Desaplica la textura actual
void TextureManager::noTexture() {
	glBindTexture(GL_TEXTURE_2D, 0);
}


void TextureManager::initialize() {
	TextureManager::blockAtlas = TextureAtlas(8, 3);
	TextureManager::fontAtlas = TextureAtlas(16, 16);
}

bool TextureManager::getBlockTexCoords(int texNum, std::array<float, 4> &texCoords) {
	return TextureManager::blockAtlas.getCoords(texNum, texCoords);
}

bool TextureManager::getFontTexCoords(char c, std::array<float, 4> &fontTexCoords) {
	TextureManager::fontAtlas.getCoords((int)c - 32, fontTexCoords);
	float offset = (0.1f / (float)TextureManager::fontAtlas.getSizeX());
	fontTexCoords[0] -= offset;
	fontTexCoords[2] -= offset;
	return true;
}