#include "ChunkMesh.h"

ChunkMesh::ChunkMesh() {
	oMesh = Mesh(Primitiva::QUAD);
	tMesh = Mesh(Primitiva::QUAD);
	lMesh = Mesh(Primitiva::LINIA);
}

//Estructura vertex: x,y,z, nx,ny,nz, R,G,B,A, tx,ty (12 floats)
void ChunkMesh::addVertexO(float* vert, float* norm, float* col, float* text, Primitiva prim) {
	switch (prim) {
	case Primitiva::QUAD:
		oMesh.addVertex(vert, norm, col, text);
		break;
	case Primitiva::LINIA:
		lMesh.addVertex(vert, norm, col, text);
		break;
	}
}

//Estructura vertex: x,y,z, nx,ny,nz, R,G,B,A, tx,ty (12 floats)
void ChunkMesh::addVertexT(float* vert, float* norm, float* col, float* text) {
	tMesh.addVertex(vert, norm, col, text);
}


void ChunkMesh::drawO() {
	oMesh.draw();
	glLineWidth(2.5f);
	lMesh.draw();
	glLineWidth(1.0f);
}

void ChunkMesh::drawT() {
	//glDisable(GL_CULL_FACE);
	tMesh.draw();
	//glEnable(GL_CULL_FACE);
}

void ChunkMesh::erase() {
	oMesh.erase();
	lMesh.erase();
	tMesh.erase();
}

void ChunkMesh::eraseT() {
	tMesh.erase();
}

void ChunkMesh::eraseO() {
	oMesh.erase();
	lMesh.erase();
}


ChunkMesh::~ChunkMesh() {
	this->erase();
}

void ChunkMesh::update() {
	oMesh.update();
	lMesh.update();
	tMesh.update();
}

void ChunkMesh::updateO() {
	oMesh.update();
	lMesh.update();
}

void ChunkMesh::updateT() {
	tMesh.update();
}
