#include "ShaderManager.h"
#include "GL/glew.h"

unordered_map<string, Shader> ShaderManager::shaders;

void ShaderManager::initialize() {
	//
}

void ShaderManager::addShader(string sh, std::string nom) {
	shaders[sh] = Shader(nom);
}

void ShaderManager::useShader(string sh) {
	shaders[sh].use();
}

unsigned int ShaderManager::getShader(string sh) {
	return shaders[sh].getProgram();
}