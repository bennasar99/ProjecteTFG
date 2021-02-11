#include <GL/glew.h>
#include "Mesh.h"

Mesh::Mesh() {
	this->prim = Primitiva::TRIANGLE;
	this->vbo = 0;
	this->vao = 0;
}

Mesh::Mesh(Primitiva prim) {
	this->prim = prim;
	this->vbo = 0;
	this->vao = 0;
}

//Estructura vertex: x,y,z, nx,ny,nz, R,G,B,A, tx,ty (12 floats)
void Mesh::addVertex(float* vert, float* norm, float* col, float* text) {
	for (int i = 0; i < 3; i++) {
		this->vert.push_back(vert[i]);
	}
	for (int i = 0; i < 3; i++) {
		this->vert.push_back(norm[i]);
	}
	for (int i = 0; i < 4; i++) {
		this->vert.push_back(col[i]);
	}
	for (int i = 0; i < 2; i++) {
		this->vert.push_back(text[i]);
	}
}

void Mesh::update() {
	//Eliminam el VAO i el VBO
	glDeleteBuffers(1, &this->vbo);
	glDeleteVertexArrays(1, &this->vao);

	//Generam el VAO i el VBO
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);

	//Mides array
	size_t vS = this->vert.size() * sizeof(float);

	glBindVertexArray(this->vao); //Attach al VAO

	//Preparació VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, vS, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vS, this->vert.data());
	
	//Punters VAO
	void* nO = (void*)(3 * sizeof(float));
	void* cO = (void*)(6 * sizeof(float));
	void* tO = (void*)(10 * sizeof(float));
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(float) * 12, 0);
	glNormalPointer(GL_FLOAT, sizeof(float) * 12, nO);
	glColorPointer(4, GL_FLOAT, sizeof(float) * 12, cO);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 12, tO);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //Deattach VBO
	glBindVertexArray(0); //Deattach VAO

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}

void Mesh::draw() {
	glBindVertexArray(this->vao);

	//size dividit entre 12 per què hem de tenir en compte totes les dades (3 pos + 3 normal + 4 color + 2 text)
	switch (this->prim) {
	case Primitiva::QUAD:
		glDrawArrays(GL_QUADS, 0, this->vert.size() / 12);
		break;
	case Primitiva::LINIA:
		glDrawArrays(GL_LINES, 0, this->vert.size() / 12);
		break;
	case Primitiva::TRIANGLE:
		glDrawArrays(GL_TRIANGLES, 0, this->vert.size() / 12);
		break;
	}

	glBindVertexArray(0);
}

void Mesh::erase() {
	this->vert.clear();
	this->norm.clear();
	this->col.clear();
	this->text.clear();
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