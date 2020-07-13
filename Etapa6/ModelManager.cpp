#include "GL/glew.h"
#include "ModelManager.h"

GLuint ModelManager::mod[NMOD];
GLuint ModelManager::ind[NMOD];

//Carrega una textura, assignant-la a una "Textura" (enum) concreta
void ModelManager::drawModel(Model nom) {
	int num = static_cast<int>(nom);
	glBindBuffer(GL_ARRAY_BUFFER, mod[num]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind[num]);
	// enable vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	switch (nom) {
	case Model::CUB:

		size_t nOffset = sizeof(GLfloat) * 12 * 6;
		size_t tOffset = nOffset + sizeof(GLfloat) * 12 * 6;

		// specify vertex arrays with their offsets
		glVertexPointer(3, GL_FLOAT, 0, (void*)0);
		glNormalPointer(GL_FLOAT, 0, (void*)nOffset);
		//glColorPointer(3, GL_FLOAT, 0, (void*)cOffset);
		glTexCoordPointer(2, GL_FLOAT, 0, (void*)tOffset);

		glFrontFace(GL_CCW);
		// finally draw a cube with glDrawElements()
		glDrawElements(GL_TRIANGLES,            // primitive type
			36,                      // # of indices
			GL_UNSIGNED_INT,         // data type
			(void*)0);               // offset to indices
	}

	// disable vertex arrays
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

//Inicialització
void ModelManager::initialize(){
	
	GLfloat vert[] = {
	 .5f, .5f, .5f,  -.5f, .5f, .5f,  -.5f,-.5f, .5f,  .5f,-.5f, .5f, // v0,v1,v2,v3 (front)
	 .5f, .5f, .5f,   .5f,-.5f, .5f,   .5f,-.5f,-.5f,  .5f, .5f,-.5f, // v0,v3,v4,v5 (right)
	 .5f, .5f, .5f,   .5f, .5f,-.5f,  -.5f, .5f,-.5f, -.5f, .5f, .5f, // v0,v5,v6,v1 (top)
	-.5f, .5f, .5f,  -.5f, .5f,-.5f,  -.5f,-.5f,-.5f, -.5f,-.5f, .5f, // v1,v6,v7,v2 (left)
	-.5f,-.5f,-.5f,   .5f,-.5f,-.5f,   .5f,-.5f, .5f, -.5f,-.5f, .5f, // v7,v4,v3,v2 (bottom)
	 .5f,-.5f,-.5f,  -.5f,-.5f,-.5f,  -.5f, .5f,-.5f,  .5f, .5f,-.5f  // v4,v7,v6,v5 (back)
	};

	// normal array
	GLfloat normals[] = {
		 0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,  // v0,v1,v2,v3 (front)
		 1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,  // v0,v3,v4,v5 (right)
		 0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,  // v0,v5,v6,v1 (top)
		-1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  // v1,v6,v7,v2 (left)
		 0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,  // v7,v4,v3,v2 (bottom)
		 0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1   // v4,v7,v6,v5 (back)
	};

	// texture coord array
	GLfloat texCoords[] = {
		1, 0,   0, 0,   0, 1,   1, 1,               // v0,v1,v2,v3 (front)
		0, 0,   0, 1,   1, 1,   1, 0,               // v0,v3,v4,v5 (right)
		1, 1,   1, 0,   0, 0,   0, 1,               // v0,v5,v6,v1 (top)
		1, 0,   0, 0,   0, 1,   1, 1,               // v1,v6,v7,v2 (left)
		0, 1,   1, 1,   1, 0,   0, 0,               // v7,v4,v3,v2 (bottom)
		0, 1,   1, 1,   1, 0,   0, 0                // v4,v7,v6,v5 (back)
	};

	GLuint indices[] = {
	 0, 1, 2,   2, 3, 0,    // v0-v1-v2, v2-v3-v0 (front)
	 4, 5, 6,   6, 7, 4,    // v0-v3-v4, v4-v5-v0 (right)
	 8, 9,10,  10,11, 8,    // v0-v5-v6, v6-v1-v0 (top)
	12,13,14,  14,15,12,    // v1-v6-v7, v7-v2-v1 (left)
	16,17,18,  18,19,16,    // v7-v4-v3, v3-v2-v7 (bottom)
	20,21,22,  22,23,20     // v4-v7-v6, v6-v5-v4 (back)
	};

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &mod[0]);
	glGenBuffers(1, &ind[0]);

	// copy vertex attribs data to VBO
	glBindBuffer(GL_ARRAY_BUFFER, mod[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert) + sizeof(normals) /*+ cSize*/ + sizeof(texCoords), 0, GL_STATIC_DRAW); // reserve space
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert), vert);                  // copy verts at offset 0
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vert), sizeof(normals), normals);               // copy norms after verts
	//glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize, cSize, colors);          // copy cols after norms
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vert) + sizeof(normals) /*+ cSize*/, sizeof(texCoords), texCoords); // copy texs after cols
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// copy index data to VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}