#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include "Mesh.h"

class ChunkMesh {
private:
	Mesh oMesh; //Quads sòlids
	Mesh trMesh; //Quads transparents
	Mesh lMesh; //Mesh de línies
	Mesh tMesh;  //Mesh de triangles

	unsigned int vbo;
	Primitiva prim;

public:
	void addVertexO(unsigned short* vert, unsigned short* norm, unsigned char* col, unsigned short* text, Primitiva prim);
	void addVertexT(unsigned short* vert, unsigned short* norm, unsigned char* col, unsigned short* text);

	void drawO();
	void drawT();

	void update();
	void updateO();
	void updateT();
	void erase();
	void eraseT();
	void eraseO();

	ChunkMesh();
	~ChunkMesh();
};