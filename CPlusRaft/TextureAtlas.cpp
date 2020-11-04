#include "TextureAtlas.h"

TextureAtlas::TextureAtlas(int sizeX, int sizeY) {
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	
	this->uSizeX = 1.0f / (float)sizeX;
	this->uSizeY = 1.0f / (float)sizeY;
}

TextureAtlas::TextureAtlas() {
	this->sizeX = 1;
	this->sizeY = 1;

	this->uSizeX = 1;
	this->uSizeY = 1;
}

float TextureAtlas::getCoordX(int index) {
	return ((index % this->sizeX) * this->uSizeX);
}

float TextureAtlas::getCoordY(int index) {
	return ((index / this->sizeX) * this->uSizeY);
}

float* TextureAtlas::getCoords(int index) {
	float x = this->getCoordX(index);
	float y = this->getCoordY(index);
	float coords[4] = { x, y, x + uSizeX, y + uSizeY }; //Coordenades bottom i top (xb, yb, xt, yt)
	return coords;
}