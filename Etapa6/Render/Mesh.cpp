#include <GL/glew.h>
#include "Mesh.h"

Mesh::Mesh() {
	
}

void Mesh::addVertex(float* vert, float* norm, float* col, float* text) {
	for (int i = 0; i < 3; i++) {
		this->vert.push_back(vert[i]);
	}
	for (int i = 0; i < 3; i++) {
		this->text.push_back(norm[i]);
	}
	for (int i = 0; i < 4; i++) {
		this->text.push_back(col[i]);
	}
	for (int i = 0; i < 2; i++) {
		this->text.push_back(text[i]);
	}
}

void Mesh::buildVBO() {

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &this->vbo);

	//Mides array
	size_t vS = this->vert.size() * sizeof(float);
	size_t nS = this->norm.size() * sizeof(float);
	size_t cS = this->col.size() * sizeof(float);
	size_t tS = this->text.size() * sizeof(float);

	// copy vertex attribs data to VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, vS + nS +  cS + tS, 0, GL_STATIC_DRAW); // reserve space
	glBufferSubData(GL_ARRAY_BUFFER, 0, vS, this->vert.data());                  // copy verts at offset 0
	glBufferSubData(GL_ARRAY_BUFFER, vS, this->norm.size() * sizeof(float), this->norm.data());               // copy norms after verts
	glBufferSubData(GL_ARRAY_BUFFER, vS + nS, cS, this->col.data());          // copy cols after norms
	glBufferSubData(GL_ARRAY_BUFFER, vS + nS + cS, tS, this->text.data()); // copy texs after cols
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Mesh::draw() {
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	// enable vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	size_t nOffset = sizeof(GLfloat) * 12 * 6;
	size_t tOffset = nOffset + sizeof(GLfloat) * 12 * 6;

	//Càlcul offsets:
	void* nO = (void*)(this->vert.size() * sizeof(float));
	void* cO = (void*)(this->norm.size() * sizeof(float) + this->vert.size() * sizeof(float));
	void* tO = (void*)(this->col.size() * sizeof(float) + this->norm.size() * sizeof(float) + this->vert.size() * sizeof(float));

	// specify vertex arrays with their offsets
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glNormalPointer(GL_FLOAT, 0, nO);
	glColorPointer(3, GL_FLOAT, 0, cO);
	glTexCoordPointer(2, GL_FLOAT, 0, tO);

	//glFrontFace(GL_CCW);
	// finally draw a cube with glDrawElements()
	glDrawArrays(GL_TRIANGLES,            // primitive type
		0,                      // # of indices
		this->vert.size() / 3);         // data type



	// disable vertex arrays
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::erase() {
	this->vert.empty();
	this->norm.empty();
	this->col.empty();
	this->text.empty();
}

//Retorna la mida (nº vèrtexos)
int Mesh::getSize() {
	return this->vert.size() / 3;
}

float* Mesh::getVertexData() {
	return this->vert.data();
}

float* Mesh::getTexCoords() {
	return this->text.data();
}

float* Mesh::getColors() {
	return this->col.data();
}

float* Mesh::getNormals() {
	return this->norm.data();
}