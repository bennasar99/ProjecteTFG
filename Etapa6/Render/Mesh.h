#pragma once
#include <vector>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

class Mesh {
private:
	std::vector<float> vert;
	std::vector<float> text;
	std::vector<float> col;
	std::vector<float> norm;

	unsigned int vbo;

public:
	void addVertex(float* vert, float* text, float* col, float* norm);

	void buildVBO();
	void draw();

	float* getVertexData();
	float* getTexCoords();
	float* getColors();
	float* getNormals();

	int getSize();
	void erase();

	Mesh();
};