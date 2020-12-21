#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include "Mesh.h"

class ChunkMesh {
private:
	Mesh* oMesh; //Quads sòlids
	Mesh* tMesh; //Quads transparents
	Mesh* lMesh; //Mesh de línies

	unsigned int vbo;
	Primitiva prim;

public:
	void addVertexO(float* vert, float* norm, float* col, float* text, Primitiva prim);
	void addVertexT(float* vert, float* norm, float* col, float* text);

	void drawO();
	void drawT();

	void update();
	void erase();

	ChunkMesh();
};