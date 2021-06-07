#pragma once
#include <climits>
#include <vector>
#include <ctype.h>
#include <unordered_map>
#include "../Render/Shader.h"

#define NSHADERS 1
class Mesh;

class ShaderManager {
private:

	static unordered_map<string, Shader> shaders;

	ShaderManager() {}

public:

	static void addShader(string nom, std::string path);
	static void useShader(string nom);
	static unsigned int getShader(string nom);
	static void initialize();

};

