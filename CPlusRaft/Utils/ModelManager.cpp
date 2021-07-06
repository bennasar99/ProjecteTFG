#include "ModelManager.h"
#include "GL/glew.h"
#include "../Render/Mesh.h"

unordered_map<string, Mesh*> ModelManager::model;

void ModelManager::initialize() {
	//
}

void ModelManager::addModel(string mod, int frames, std::string path) {

	model[mod] = new Mesh[frames];
	Mesh* model = ModelManager::model[mod];

	if (frames <= 1) { //Si només té un frame carregam directament amb el path
		model[0] = Mesh(path);
		model[0].update();
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
		//printf("Model a %s\n", pathN.c_str());
		model[i] = Mesh(pathN);
		model[i].update();
	}
	

	

}

void ModelManager::drawModel(string mod, int frame) {
	model[mod][frame].draw();
}
