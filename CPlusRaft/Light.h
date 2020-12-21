#pragma once
#include "Vector3.h"
#include <string.h>

enum class Attenuation { //Tipus d'atenuació
	CONSTANT,
	LINEAR,
	QUADRATIC
};

/*
	Classe Light: representa una llum, amb la seva informació
*/
class Light {
private:
	float spotDir[3] = { 0,0,-1 };
	float pos[4] = {0, 0, 0, 1};
	float color[4] = { 1,1,1,1 };
	float specular[4] = { 0,0,0,0 };
	float attenuation[3] = { 1, 1, 1 }; //Atenuació (3 tipus)
	float spreadAngle = 180; //Spot cuttoff
	float dist = 1000; //Distància de la llum a la càmera
	float concentration = 0; //Concentració de la llum
	float specularExponent = 0.1f; //Specular = color * exponent

	bool enabled = true; //Indica si la llum està encesa
public:
	void setPos(Vector3<float> pos);
	void setPos(float pos[4]);
	float* getPos();
	Vector3<float> getPosVec();

	void setDir(Vector3<float> dir);
	void setDir(float dir[3]);
	float* getDir();

	void setAttenuation(Attenuation at, float value);
	float getAttenuation(Attenuation at);

	float getDist();
	void setDist(float dist);

	void setSpreadAngle(float angle);
	float getSpreadAngle();

	void setLightConcentration(float angle);
	float getLightConcentration();

	void setEnabled(bool enabled);
	bool getEnabled();

	void setColor(float r, float g, float b, float a);
	void setColor(float color[4]);
	float* getColor();
	float* getSpecular();

	bool operator<(const Light& other);

	Light(Vector3<float> pos);
};