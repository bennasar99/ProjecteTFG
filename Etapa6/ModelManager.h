#pragma once
#include <list>
#include "Utils.h"

#define NMOD 1 //Nombre de models

enum class Model {
	CUB
};

/*
	S'encarrega de gestionar les textures
*/
class ModelManager {
private:
	static GLuint mod[NMOD]; //Textures
	static GLuint ind[NMOD]; //Índexos

	ModelManager() {}
public:
	static void initialize();
	static void drawModel(Model nom);
};