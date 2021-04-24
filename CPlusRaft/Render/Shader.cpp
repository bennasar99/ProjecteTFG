#include <GL/glew.h>
#include "Shader.h"

Shader::Shader() {

}

Shader::Shader(std::string name) {

	unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
    unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);

    std::string pathF = "./shaders/" + name + ".fs";
    std::string pathV = "./shaders/" + name + ".vs";

    loadFromFile(pathF, frag);
    loadFromFile(pathV, vert);

    this->shader = glCreateProgram();
    glAttachShader(this->shader, vert);
    glAttachShader(this->shader, frag);
    glLinkProgram(this->shader);

    glDeleteShader(shader);
}

bool Shader::loadFromFile(string path, unsigned int shader) {
    const char** source = new const char*;
    int len;

    if (!Shader::readFile(path, source, &len)) {
        printf("Fitxer de shader invàlid\n");
        return false;
    }

    glShaderSourceARB(shader, 1, source, &len);

    //glCompileShaderARB(vertex);
    glCompileShaderARB(shader);


    GLint compiled;

    glGetObjectParameterivARB(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled)
    {
        printf("Compilació exitosa de shader %s\n", path.c_str());
    }
    else {
        printf("Compilació fàllida de shader %s\n", path.c_str());
    }

    return true;
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
    glUseProgram(this->shader);
}

unsigned int Shader::getProgram() {
    return this->shader;
}