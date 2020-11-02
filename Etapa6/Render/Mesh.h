#pragma once
#include <vector>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

enum class Primitiva {
	LINIA,
	QUAD,
	TRIANGLE
};

class Mesh {
private:
	std::vector<float> vert;
	std::vector<float> text;
	std::vector<float> col;
	std::vector<float> norm;

	unsigned int vbo;
	Primitiva prim;

public:
	void addVertex(float* vert, float* norm, float* col, float* text);

	void buildVBO();
	void draw();

	float* getVertexData();
	float* getTexCoords();
	float* getColors();
	float* getNormals();

	int getSize();
	void erase();

	Mesh(Primitiva prim);
};