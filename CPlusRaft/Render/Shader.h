#pragma once
#include <string>
#include <iostream>
#include <istream>
#include <fstream>


using namespace std;

class Shader {
private:
	unsigned int shader;

	bool loadFromFile(string path, unsigned int shader);
	bool readFile(string path, const char** source, int* len);

public:
	void use();

	unsigned int getProgram();

	Shader(std::string name);
	Shader();
};