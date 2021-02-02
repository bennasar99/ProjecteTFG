#include <GL/glew.h>
#include "Shader.h"

Shader::Shader(unsigned int tipus, const char* path) {
    if (tipus != GL_VERTEX_SHADER && tipus != GL_FRAGMENT_SHADER) {
        printf("tipus invalid de Shader\n");
    }
	shader = glCreateShader(tipus);

    const char** source = new const char*;
    int len;

    if (!Shader::readFile("./shaders/" + string(path), source, &len)) {
        printf("Fitxer de shader invàlid\n");
    }

	glShaderSourceARB(shader, 1, source, &len);
    
    //glCompileShaderARB(vertex);
    glCompileShaderARB(shader);


    GLint compiled;

    glGetObjectParameterivARB(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled)
    {
        printf("Compilació exitosa de shader %s\n", path);
    }
    else {
        printf("Compilació fàllida de shader %s\n", path);
    }
}

bool Shader::readFile(string path, const char** source, int* len) {
    std::ifstream file(path);
    file.seekg(0, std::ios::end);
    *len = file.tellg();

    std::string buffer(*len, ' ');
    file.seekg(0);
    file.read(&buffer[0], *len);
    file.close();
    
    *source = buffer.c_str();
    return true;
}

void Shader::use() {
    auto ProgramObject = glCreateProgram();

    glAttachShader(ProgramObject, this->shader);

    glLinkProgram(ProgramObject);
    glUseProgram(ProgramObject);
}