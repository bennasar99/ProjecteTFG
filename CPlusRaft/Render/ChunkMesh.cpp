#include "ChunkMesh.h"

ChunkMesh::ChunkMesh() {
	oMesh = new Mesh(Primitiva::QUAD);
	tMesh = new Mesh(Primitiva::QUAD);
	lMesh = new Mesh(Primitiva::LINIA);
}

//Estructura vertex: x,y,z, nx,ny,nz, R,G,B,A, tx,ty (12 floats)
void ChunkMesh::addVertexO(float* vert, float* norm, float* col, float* text, Primitiva prim) {
	switch (prim) {
	case Primitiva::QUAD:
		this->oMesh->addVertex(vert, norm, col, text);
		break;
	case Primitiva::LINIA:
		this->lMesh->addVertex(vert, norm, col, text);
		break;
	}
}

//Estructura vertex: x,y,z, nx,ny,nz, R,G,B,A, tx,ty (12 floats)
void ChunkMesh::addVertexT(float* vert, float* norm, float* col, float* text) {
	this->tMesh->addVertex(vert, norm, col, text);
}


void ChunkMesh::drawO() {
	oMesh->draw();
	glLineWidth(2.5f);
	lMesh->draw();
	glLineWidth(1.0f);
}

void ChunkMesh::drawT() {
	glDisable(GL_CULL_FACE);
	tMesh->draw();
	glEnable(GL_CULL_FACE);
}

void ChunkMesh::erase() {
	oMesh->erase();
	tMesh->erase();
	lMesh->erase();
}

void ChunkMesh::update() {
	oMesh->update();
	tMesh->update();
	lMesh->update();
}