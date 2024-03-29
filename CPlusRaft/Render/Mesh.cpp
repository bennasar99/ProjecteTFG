#include <GL/glew.h>
#include "Mesh.h"
#include "../Utils/RenderManager.h"
#include "OBJ_Loader.h"

Mesh::Mesh() {
	this->prim = Primitiva::TRIANGLE;
	this->vbo = 0;
	this->vao = 0;
	this->vert.shrink_to_fit();
}

Mesh::Mesh(std::string path) { //C�rrega de fitxer
	this->prim = Primitiva::TRIANGLE;
	this->vbo = 0;
	this->vao = 0;
	this->vert.shrink_to_fit();

	objl::Loader loader;
	loader.LoadFile(path);
	for (int k = 0; k < loader.LoadedMeshes.size(); k++) {
		objl::Mesh mesh = loader.LoadedMeshes[k];
		for (int j = 0; j < mesh.Indices.size(); j++) {
			int i = mesh.Indices.at(j);
			objl::Vector3 vert = mesh.Vertices[i].Position;
			unsigned short vF[3] = { toHFloat(vert.X), toHFloat(vert.Y), toHFloat(vert.Z) };
			objl::Vector3 norm = mesh.Vertices[i].Normal;
			unsigned short nF[3] = { toHFloat(norm.X), toHFloat(norm.Y), toHFloat(norm.Z) };
			objl::Vector2 text = mesh.Vertices[i].TextureCoordinate;
			unsigned short tF[2] = { toHFloat(text.X), toHFloat(1-text.Y) };
			unsigned char cF[4] = {255, 255, 255, 255 };
			this->addVertex(vF, nF, cF, tF);
		}
	}
}

Mesh::Mesh(Primitiva prim) {
	this->prim = prim;
	this->vbo = 0;
	this->vao = 0;
	this->vert.shrink_to_fit();
}

Mesh::~Mesh() {
	if (this->vao != 0) {
		RenderManager::removeVAO(this->vao);
	}
	if (this->vbo != 0) {
		RenderManager::removeVBO(this->vbo);
	}
}

//Estructura vertex: x,y,z, nx,ny,nz, R,G,B,A, tx,ty (12 floats)
void Mesh::addVertex(unsigned short* vert, unsigned short* norm, unsigned char* col, unsigned short* text) {
	for (int i = 0; i < 3; i++) {
		this->vert.push_back(vert[i]);
	}
	for (int i = 0; i < 3; i++) {
		this->vert.push_back(norm[i]);
	}
	for (int i = 0; i < 2; i++) {
		this->vert.push_back(text[i]);
	}
	for (int i = 0; i < 4; i++) {
		this->col.push_back(col[i]);
	}
}

void Mesh::update() {
	if (this->vert.size() == 0) {
		return;
	}

	if (this->vao == 0) {
		glGenVertexArrays(1, &this->vao);
	}
	if (this->vbo == 0) {
		glGenBuffers(1, &this->vbo);
	}
	//Generam el VAO i el VBO

	//Mides array
	size_t vS = this->vert.size() * sizeof(unsigned short);
	size_t cS = this->col.size() * sizeof(unsigned char);

	glBindVertexArray(this->vao); //Attach al VAO

	//Preparaci� VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, vS + cS, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vS, this->vert.data());
	glBufferSubData(GL_ARRAY_BUFFER, vS, cS, this->col.data());
	
	//Punters VAO
	void* nO = (void*)(3 * sizeof(unsigned short));
	void* tO = (void*)(6 * sizeof(unsigned short));
	void* cO = (void*)(8 * sizeof(unsigned short));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	size_t sepV = sizeof(unsigned short) * 8;
	size_t sepC = sizeof(unsigned char) * 4;

	glVertexPointer(3, GL_HALF_FLOAT, (GLsizei)sepV , 0); //Min = short
	glNormalPointer(GL_HALF_FLOAT, (GLsizei)sepV, nO); //Min = byte
	glTexCoordPointer(2, GL_HALF_FLOAT, (GLsizei)sepV, tO); //Min = short
	glColorPointer(4, GL_UNSIGNED_BYTE, (GLsizei)sepC, (void*)vS); //Min = (unsigned) byte

	glBindBuffer(GL_ARRAY_BUFFER, 0); //Deattach VBO
	glBindVertexArray(0); //Deattach VAO

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}

void Mesh::draw() {
	glBindVertexArray(this->vao);

	size_t count = this->vert.size() / 8;

	//size dividit entre 12 per qu� hem de tenir en compte totes les dades (3 pos + 3 normal + 4 color + 2 text)
	switch (this->prim) {
	case Primitiva::QUAD:
		glDrawArrays(GL_QUADS, 0, (GLsizei)count );
		break;
	case Primitiva::LINIA:
		glDrawArrays(GL_LINES, 0, (GLsizei)count);
		break;
	case Primitiva::TRIANGLE:
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)count);
		break;
	}

	glBindVertexArray(0);
}

void Mesh::erase() {
	this->vert.clear();
	this->vert.shrink_to_fit();
	this->col.clear();
	this->col.shrink_to_fit();
}


//Retorna la mida (n� v�rtexos)
int Mesh::getSize() {
	return (int)this->vert.size() / 3;
}

unsigned short* Mesh::getVertexData() {
	return this->vert.data();
}
