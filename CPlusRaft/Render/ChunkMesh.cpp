#include "ChunkMesh.h"

ChunkMesh::ChunkMesh() {
	tMesh = Mesh(Primitiva::TRIANGLE);
	oMesh = Mesh(Primitiva::QUAD);
	trMesh = Mesh(Primitiva::QUAD);
	lMesh = Mesh(Primitiva::LINIA);
}

//Estructura vertex: x,y,z, nx,ny,nz, R,G,B,A, tx,ty (12 floats)
void ChunkMesh::addVertexO(float* vert, float* norm, float* col, float* text, Primitiva prim) {
	switch (prim) {
	case Primitiva::QUAD:
		oMesh.addVertex(vert, norm, col, text);
		break;
	case Primitiva::TRIANGLE:
		tMesh.addVertex(vert, norm, col, text);
		break;
	case Primitiva::LINIA:
		lMesh.addVertex(vert, norm, col, text);
		break;
	}
}

//Estructura vertex: x,y,z, nx,ny,nz, R,G,B,A, tx,ty (12 floats)
void ChunkMesh::addVertexT(float* vert, float* norm, float* col, float* text) {
	trMesh.addVertex(vert, norm, col, text);
}


void ChunkMesh::drawO() {
	GLfloat specular[4] = { 0.1f,0.1f,0.1f,1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	tMesh.draw();
	oMesh.draw();
	glLineWidth(2.5f);
	lMesh.draw();
	glLineWidth(1.0f);
}

void ChunkMesh::drawT() {
	//glDisable(GL_CULL_FACE);
	trMesh.draw();
	//glEnable(GL_CULL_FACE);
}

void ChunkMesh::erase() {
	oMesh.erase();
	lMesh.erase();
	tMesh.erase();
	trMesh.erase();
}

void ChunkMesh::eraseT() {
	trMesh.erase();
}

void ChunkMesh::eraseO() {
	oMesh.erase();
	lMesh.erase();
	tMesh.erase();
}


ChunkMesh::~ChunkMesh() {
	this->erase();
}

void ChunkMesh::update() {
	tMesh.update();
	oMesh.update();
	lMesh.update();
	trMesh.update();
}

void ChunkMesh::updateO() {
	oMesh.update();
	lMesh.update();
	tMesh.update();
}

void ChunkMesh::updateT() {
	trMesh.update();
}

void ChunkMesh::destroy() {
	tMesh.destroy();
	oMesh.destroy();
	lMesh.destroy();
	trMesh.destroy();
}