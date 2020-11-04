#pragma once
#include <vector>

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
	unsigned int vao;

	Primitiva prim;

public:
	void addVertex(float* vert, float* norm, float* col, float* text);

	void update();
	void draw();

	float* getVertexData();
	float* getTexCoords();
	float* getColors();
	float* getNormals();

	int getSize();
	void erase();

	Mesh(Primitiva prim);
};