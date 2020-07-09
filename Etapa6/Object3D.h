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

        // �ndexos al vector de v�rtexos
        std::vector<unsigned int> vertices;
        std::vector<unsigned int> normals;
    };

    // �ndexos dels triangles
    std::vector<std::vector<unsigned int>> mIndices;

    // v�rtexos de l'objecte
    std::vector<Vector3> mVertices;
    std::vector<Vector3> mColors;

    // vectors normals a cada v�rtex
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

