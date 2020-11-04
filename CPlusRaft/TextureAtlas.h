#pragma once

/*
	S'encarrega de gestionar un atles de textures. Permet obtenir les texCoords adequades
*/
class TextureAtlas {
private:
	int sizeX, sizeY;
	float uSizeX, uSizeY; //Mida d'una unitat (1 / sizeX|Y)

public:
	//index: d'adalt cap abaix i d'esquerra cap a dreta
	float getCoordX(int index);
	float getCoordY(int index);
	float* getCoords(int index);

	TextureAtlas();
	TextureAtlas(int sizeX, int sizeY);
};