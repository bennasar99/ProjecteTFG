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

int TextureAtlas::getSizeX() {
	return sizeX;
}

int TextureAtlas::getSizeY() {
	return sizeY;
}

bool TextureAtlas::getCoords(int index, std::array<float ,4> &texCoords) {
	if (index < 0) {
		texCoords = { 0,0,0,0 };
		return false;
	}
	float x = this->getCoordX(index);
	float y = this->getCoordY(index);
	float marge = 0.00; //Per evitar "bleeding edges"
	texCoords = { x + marge, y+marge, x + uSizeX - marge, y + uSizeY - marge}; //Coordenades bottom i top (xb, yb, xt, yt)
}