#include "Material.h"
#include <fstream>
#include <iostream>
#include <exception>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

/**
 * Carrega la informació d'un material en format .mtl
 */
Material::Material(std::string nom, Vector3 ambient, Vector3 specular, float n, Vector3 diffuse) {
    this->nomMaterial = nom;
    this->ambient = ambient;
    this->specular = specular;
    this->specularExponent = n;
    this->diffuse = diffuse;
}

Material::~Material() {

}

/**
 * En un mateix arxiu hi pot haver definits diferents materials
 * Aquests materials poden tenir un nom diferents
 */
std::list<Material> Material::loadMaterialFile(std::string filename) {
    // obrir l'arxiu
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: no s'ha pogut carregar l'arxiu de material" << std::endl;
        throw new std::runtime_error("File not found");
    }

    std::list<Material> materials;
    materials.clear();

    // dades sobre el material actual
    std::string currentMaterial;
    bool materialTrobat = false;
    Vector3 ambient;
    Vector3 specular;
    float specularExponent;
    Vector3 diffuse;

    // inicialment no s'ha trobat cap material
    materialTrobat = false;

    // llegir línia a línia de l'arxiu
    std::string line;
    while (std::getline(file, line)) {
        std::cout << "llegint linia " << line << std::endl;
        size_t npos;
        if ((npos = line.find("newmtl")) != std::string::npos) {
            // es defineix un nou material
            npos = line.find(" ");
            currentMaterial = line.substr(npos + 1, line.length() - 1);
            materialTrobat = true;
        }
        else if (line.find("Ns") != std::string::npos) {
            sscanf_s(line.c_str(), "%*s %f", &specularExponent);
            std::cout << "Ns = " << specularExponent << std::endl;
        }
        else if (line.find("Ka") != std::string::npos) {
            sscanf_s(line.c_str(), "%*s %f %f %f", &ambient.x, &ambient.y, &ambient.z);
        }
        else if (line.find("Ks") != std::string::npos) {
            sscanf_s(line.c_str(), "%*s %f %f %f", &specular.x, &specular.y, &specular.z);
        }
        else if (line.find("Kd") != std::string::npos) {
            sscanf_s(line.c_str(), "%*s %f %f %f", &diffuse.x, &diffuse.y, &diffuse.z);
        }
        else if (line == "") {
            if (materialTrobat) {
                // final del material
                std::cout << "Material final" << std::endl;
                Material m(currentMaterial, ambient, specular, specularExponent, diffuse);
                materials.push_back(m);
                materialTrobat = false;
            }
        }
    }

    if (materialTrobat) {
        // és possible que el fitxer no acabi amb una línia buida
        Material m(currentMaterial, ambient, specular, specularExponent, diffuse);
        materials.push_back(m);
        materialTrobat = false;
    }

    return materials;
}

/**
 * Configura OpenGL per utilitzar aquest material
 */
void Material::setMaterial() {
    glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
    glColor3f(this->specular.x/10, this->specular.y/10, this->specular.z/10); //Reduim l'especular dels objectes

    glMaterialf(GL_SHININESS, GL_FRONT_AND_BACK, this->specularExponent);

    //No ens interessa carregar la component d'ambient
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
    glColor3f(this->ambient.x, this->ambient.y, this->ambient.z);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(this->diffuse.x, this->diffuse.y, this->diffuse.z);

    glColorMaterial(GL_FRONT_AND_BACK,GL_NONE);
}

std::string& Material::getNom() {
    return this->nomMaterial;
}