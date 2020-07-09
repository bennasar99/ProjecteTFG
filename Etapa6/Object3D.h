#pragma once

#include <string>
#include <fstream>
#include <vector>
#include "Vector3.h"
#include "Vector2.h"
#include "Material.h"
#include <map>

/*
    Representa un obtecte tridimensional, s'utilitza per carregar objectes importats
*/
class Object3D
{
private:
    struct Face {
        // material de la cara
        std::string material;

        // índexos al vector de vèrtexos
        std::vector<unsigned int> vertices;
        std::vector<unsigned int> normals;
    };

    // índexos dels triangles
    std::vector<std::vector<unsigned int>> mIndices;

    // vèrtexos de l'objecte
    std::vector<Vector3> mVertices;
    std::vector<Vector3> mColors;

    // vectors normals a cada vèrtex
    std::vector<Vector3> mNormals;

    // cares de l'objecte
    std::vector<struct Face> faces;

    // tots els materials per aquest objecte
    std::map<std::string, Material> materials;

public:
	Object3D();
	~Object3D();

    void load(std::string filename);
	void draw();
};

