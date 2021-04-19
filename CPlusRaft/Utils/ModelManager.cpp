#include "ModelManager.h"
#include "GL/glew.h"
#include "../Render/Mesh.h"

Mesh *ModelManager::model[NMODELS];

void ModelManager::initialize() {
	//
}

void ModelManager::addModel(Model mod, int frames, std::string path) {
	model[static_cast<int>(mod)] = new Mesh[frames];
	if (frames <= 1) { //Si només té un frame carregam directament amb el path
		model[static_cast<int>(mod)][0] = Mesh(path);
		model[static_cast<int>(mod)][0].update();
		return;
	}

	//Si no, al path se li ha d'afegir _XXXXXX per el frame corresponent
	for (int i = 0; i < frames; i++) {
		std::string num = std::to_string(i);
		std::string pathN = path;
		pathN += "_";
		for (int j = 0; j < 6 - num.length(); j++) {
			pathN += "0";
		}
		pathN += num;
		pathN += ".obj";
		printf("Model a %s\n", pathN.c_str());
		model[static_cast<int>(mod)][i] = Mesh(pathN);
		model[static_cast<int>(mod)][i].update();
	}
	

	

}

void ModelManager::drawModel(Model mod, int frame) {
	model[static_cast<int>(mod)][frame].draw();
}
