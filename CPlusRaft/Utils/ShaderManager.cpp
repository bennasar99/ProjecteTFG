#include "ShaderManager.h"
#include "GL/glew.h"

Shader ShaderManager::shaders[NSHADERS];

void ShaderManager::initialize() {
	//
}

void ShaderManager::addShader(TipusShader sh, std::string nom) {
	shaders[static_cast<int>(sh)] = Shader(nom);
}

void ShaderManager::useShader(TipusShader sh) {
	shaders[static_cast<int>(sh)].use();
}

unsigned int ShaderManager::getShader(TipusShader sh) {
	return shaders[static_cast<int>(sh)].getProgram();
}