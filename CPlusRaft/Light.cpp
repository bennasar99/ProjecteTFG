#include "Light.h"

Light::Light(Vector3<float> pos) {
	this->setPos(pos);
	this->dist = 10000;
	this->setColor(1, 1, 1, 1); //Llum blanca per defecte
}

//Getters i setters:
float* Light::getPos() {
	return this->pos;
}

Vector3<float> Light::getPosVec() {
	return Vector3(this->pos[0], this->pos[1], this->pos[2]);
}

void Light::setPos(Vector3<float> pos) {
	this->pos[0] = pos.x;
	this->pos[1] = pos.y;
	this->pos[2] = pos.z;
	this->pos[3] = 1.0f;
}

void Light::setPos(float pos[4]) {
	memcpy(this->pos, pos, 4 * sizeof(float));
}

float* Light::getDir() {
	return this->spotDir;
}

void Light::setDir(Vector3<float> dir) {
	this->spotDir[0] = dir.x;
	this->spotDir[1] = dir.y;
	this->spotDir[2] = dir.z;
}

void Light::setColor(float color[4]) {
	memcpy(this->color, color, 4 * sizeof(float));
}

float* Light::getColor() {
	return this->color;
}

float* Light::getSpecular() {
	return this->specular;
}

void Light::setColor(float r, float g, float b, float a) {
	this->color[0] = r;
	this->color[1] = g;
	this->color[2] = b;
	this->color[3] = a;
	this->specular[0] = color[0] * specularExponent;
	this->specular[1] = color[1] * specularExponent;
	this->specular[2] = color[2] * specularExponent;
	this->specular[3] = color[3] * specularExponent;
}

void Light::setDir(float dir[3]) {
	memcpy(this->spotDir, dir, 3 * sizeof(float));
}

void Light::setAttenuation(Attenuation at, float value) {
	switch (at) {
	case Attenuation::CONSTANT:
		this->attenuation[0] = value;
		break;
	case Attenuation::LINEAR:
		this->attenuation[1] = value;
		break;
	case Attenuation::QUADRATIC:
		this->attenuation[2] = value;
		break;
	}
}

float Light::getAttenuation(Attenuation at) {
	float retorn = -1.0f;
	switch (at) {
	case Attenuation::CONSTANT:
		retorn = this->attenuation[0];
		break;
	case Attenuation::LINEAR:
		retorn = this->attenuation[1];
		break;
	case Attenuation::QUADRATIC:
		retorn = this->attenuation[2];
		break;
	}
	return retorn;
}

float Light::getDist() {
	return this->dist;
}

void Light::setDist(float dist) {
	this->dist = dist;
}

void Light::setSpreadAngle(float angle) {
	this->spreadAngle = angle;
}

float Light::getSpreadAngle() {
	return this->spreadAngle;
}

void Light::setLightConcentration(float conc) {
	this->concentration = conc;
}

float Light::getLightConcentration() {
	return this->concentration;
}

void Light::setEnabled(bool enabled) {
	this->enabled = enabled;
}

bool Light::getEnabled() {
	return this->enabled;
}

//Funció de comparació '<' entre dues llums, segons distància a la càmera
bool Light::operator<(const Light& other) {
	return this->dist < other.dist;
}