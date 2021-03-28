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

	unsigned int vbo;
	unsigned int vao;

	Primitiva prim;

public:
	void addVertex(float* vert, float* norm, float* col, float* text);

	void update();
	void draw();

	float* getVertexData();

	int getSize();
	void erase();
	void destroy();

	Mesh(Primitiva prim);
	Mesh();
	~Mesh();
};