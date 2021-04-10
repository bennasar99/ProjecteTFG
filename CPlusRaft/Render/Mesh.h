#pragma once
#include <vector>
#include "../Utils.h"

enum class Primitiva {
	LINIA,
	QUAD,
	TRIANGLE
};

class Mesh {
private:

	std::vector<unsigned short> vert;
	std::vector<GLubyte> col;

	unsigned int vbo;
	unsigned int vao;

	Primitiva prim;


public:
	void addVertex(unsigned short* vert, unsigned short* norm, unsigned char* col, unsigned short* text);

	void update();
	void draw();

	unsigned short* getVertexData();

	int getSize();
	void erase();

	Mesh(Primitiva prim);
	Mesh(std::string path);
	Mesh();
	~Mesh();
};