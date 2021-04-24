#pragma once
#include <climits>
#include <vector>
#include <ctype.h>
#include "../Render/Shader.h"

#define NSHADERS 1
class Mesh;

enum class TipusShader {
	DEFAULT
};

class ShaderManager {
private:

	static Shader shaders[NSHADERS];

	ShaderManager() {}

public:

	static void addShader(TipusShader sh, std::string path);
	static void useShader(TipusShader sh);
	static unsigned int getShader(TipusShader sh);
	static void initialize();

};

