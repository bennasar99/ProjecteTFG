#pragma once
#include "Vector3.h"
#include <string>
#include <list>

/*
	Representa un material dels objectes importats, amb les seves propietats
*/
class Material
{
private:
	// components del llum
	Vector3 ambient;
	Vector3 diffuse;
	Vector3 specular;
	float specularExponent;

	// nom del material
	std::string nomMaterial;

public:

	Material(std::string nom, Vector3 ambient, Vector3 specular, float n, Vector3 diffuse);
	~Material();

	static std::list<Material> loadMaterialFile(std::string filename);
	void setMaterial();
	std::string& getNom();
};

