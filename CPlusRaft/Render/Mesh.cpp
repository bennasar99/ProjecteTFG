#include <GL/glew.h>
#include "Mesh.h"

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
	//Generam el VAO
	glDeleteVertexArrays(1, &this->vao);
	glGenVertexArrays(1, &this->vao);

	//Generam el VBO
	glDeleteBuffers(1, &this->vbo);
	glGenBuffers(1, &this->vbo);

	//Mides array
	size_t vS = this->vert.size() * sizeof(float);

	glBindVertexArray(this->vao); //Attach al VAO

	//Preparació VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, vS, 0, GL_STATIC_DRAW); // reserve space
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
	glNormalPointer(GL_FLOAT, sizeof(float) * 12, nO); //MAY NEED A HOLE ARRAY :3
	glColorPointer(4, GL_FLOAT, sizeof(float) * 12, cO);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 12, tO);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //Deattach VBO
	glBindVertexArray(0); //Deattach VAO

}

void Mesh::draw() {
	glBindVertexArray(this->vao);
	//glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	//// enable vertex arrays
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	////Mides array
	//size_t vS = this->vert.size() * sizeof(float);
	//size_t nS = this->norm.size() * sizeof(float);
	//size_t cS = this->col.size() * sizeof(float);
	//size_t tS = this->text.size() * sizeof(float);

	////Càlcul offsets:
	//void* nO = (void*)(3 * sizeof(float));
	//void* cO = (void*)(6 * sizeof(float));
	//void* tO = (void*)(10 * sizeof(float));

	//// specify vertex arrays with their offsets
	//glVertexPointer(3, GL_FLOAT, sizeof(float) * 12, 0);
	//glNormalPointer(GL_FLOAT, sizeof(float) * 12, nO); //MAY NEED A HOLE ARRAY :3
	//glColorPointer(4, GL_FLOAT, sizeof(float) * 12, cO);
	//glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 12, tO);

	//Draw the mesh
	switch (this->prim) {
	case Primitiva::QUAD:
		glDrawArrays(GL_QUADS, 0, (GLsizei)this->vert.size() / 3);
		break;
	case Primitiva::LINIA:
		glDrawArrays(GL_LINES, 0, (GLsizei)this->vert.size() / 3);
		break;
	case Primitiva::TRIANGLE:
		glDrawArrays(GL_TRIANGLES, (GLsizei)0, this->vert.size() / 3);
		break;
	}


	//// disable vertex arrays
	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//// unbind VBOs
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

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