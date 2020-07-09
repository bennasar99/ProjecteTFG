#include "Object3D.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

#include <iostream>
#include <sstream>
#include <cassert>
#include <algorithm>

Object3D::Object3D() {

}

Object3D::~Object3D() {

}

void Object3D::load(std::string filename) {
	// Open the file
	std::ifstream meshFile(filename.c_str());

	// If we cannot open the file
	if (!meshFile.is_open()) {
		// Throw an exception and display an error message
		std::string errorMessage("Error : Cannot open the file " + filename);
		std::cerr << errorMessage << std::endl;
		throw std::runtime_error(errorMessage);
	}

	std::cout << "Carregant obj " << filename << std::endl;

	std::string buffer;
	std::string line, tmp;
	int id1, id2, id3, id4;
	int nId1, nId2, nId3, nId4;
	int tId1, tId2, tId3, tId4;
	float v1, v2, v3;
	size_t found1, found2;
	std::vector<bool> isQuad;
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2<int>> uvs;
	std::vector<unsigned int> verticesIndices;
	std::vector<unsigned int> normalsIndices;
	std::vector<unsigned int> uvsIndices;


	// aquest és el material que s'aplica a les cares
	std::string darrerMaterial;

	// carregar les dades del fitxer
	// processar el fitxer línia a línia
	while (std::getline(meshFile, buffer)) {

		std::istringstream lineStream(buffer);
		std::string word;
		lineStream >> word;
		std::transform(word.begin(), word.end(), word.begin(), ::tolower);
		if (word == "usemtl") {  // Material definition
			// totes les cares que es trobin a continuació tendran els vèrtexos
			// amb el color d'aquest material
			size_t npos = buffer.find(" ");
			darrerMaterial = buffer.substr(npos + 1, buffer.size() - 1);
			std::cout << "Trobat material " << darrerMaterial << std::endl;
		}
		else if (word == "mtllib") {
			// carregar arxiu de materials
			size_t npos = buffer.find(" ");
			std::string materialsFile = buffer.substr(npos + 1, buffer.size() - 1);

			// obtenir el path correcte
			unsigned int darreraBarra = 0;
			for (int i = (int)filename.size() - 1; i >= 0; i--) {
				if (filename[i] == '/' || filename[i] == '\\') {
					darreraBarra = i + 1;
					break;
				}
			}

			std::string materialPath = filename.substr(0, darreraBarra) + materialsFile;
			std::cout << "Path = " << materialPath << std::endl;

			std::list<Material> materials = Material::loadMaterialFile(materialPath);
			std::list<Material>::iterator it = materials.begin();
			while (it != materials.end()) {
				Material& m = *it;
				std::cout << "recuperat material " << m.getNom() << std::endl;
				this->materials.emplace(m.getNom(), m);
				it++;
			}
		}
		else if (word == "v") {  // Vertex position
			sscanf_s(buffer.c_str(), "%*s %f %f %f", &v1, &v2, &v3);
			vertices.push_back(Vector3(v1, v2, v3));
		}
		else if (word == "vt") { // Vertex texture coordinate
			sscanf_s(buffer.c_str(), "%*s %f %f", &v1, &v2);
			uvs.push_back(Vector2<int>((int)v1, (int)v2));
		}
		else if (word == "vn") { // Vertex normal
			sscanf_s(buffer.c_str(), "%*s %f %f %f", &v1, &v2, &v3);
			normals.push_back(Vector3(v1, v2, v3));
		}
		else if (word == "f") { // Face
			line = buffer;
			found1 = (int)line.find("/");
			bool isFaceQuad = false;
			int foundNext = (int)line.substr(found1 + 1).find("/");

			// If the face definition is of the form "f v1 v2 v3 v4"
			if (found1 == std::string::npos) {
				int nbVertices = sscanf_s(buffer.c_str(), "%*s %d %d %d %d", &id1, &id2, &id3, &id4);
				if (nbVertices == 4) isFaceQuad = true;
			}
			// If the face definition is of the form "f v1// v2// v3// v4//"
			else if (foundNext == 0 && sscanf_s(buffer.c_str(), "%*s %d// %d// %d// %d//", &id1, &id2, &id3, &id4) >= 3) {

				int nbVertices = sscanf_s(buffer.c_str(), "%*s %d// %d// %d// %d//", &id1, &id2, &id3, &id4);
				std::cout << "Cara amb Vèrtexos: " << buffer << " ===> " << nbVertices << std::endl;
				std::cout << "Cara amb Vèrtexos: " << id1 << ", " << id2 << ", " << id3 << std::endl;

				if (nbVertices == 4) isFaceQuad = true;
			}
			else {  // If the face definition contains vertices and texture coordinates

				//get the part of the string until the second index
				tmp = line.substr(found1 + 1);
				found2 = (int)tmp.find(" ");
				tmp = tmp.substr(0, found2);
				found2 = (int)tmp.find("/");

				// If the face definition is of the form "f vert1/textcoord1 vert2/textcoord2 ..."
				if (found2 == std::string::npos) {
					int n = sscanf_s(buffer.c_str(), "%*s %d/%d %d/%d %d/%d %d/%d", &id1, &tId1, &id2, &tId2, &id3, &tId3, &id4, &tId4);
					if (n == 8) isFaceQuad = true;
					uvsIndices.push_back(tId1 - 1);
					uvsIndices.push_back(tId2 - 1);
					uvsIndices.push_back(tId3 - 1);
					if (isFaceQuad) uvsIndices.push_back(tId4 - 1);
				}
				else {
					tmp = line.substr(found1 + 1);
					found2 = (int)tmp.find("/");

					// If the face definition is of the form "f vert1/normal1 vert2/normal2 ..."
					if (found2 == 0) {
						int n = sscanf_s(buffer.c_str(), "%*s %d//%d %d//%d %d//%d %d//%d", &id1, &nId1, &id2, &nId2, &id3, &nId3, &id4, &nId4);
						
						//std::cout << "Cara amb Vèrtexos: " << id1 << ", " << id2 << ", " << id3 << "Total:" << n <<std::endl;
						struct Face tmpFace;

						// vèrtexos de la cara
						tmpFace.vertices.clear();
						tmpFace.vertices.push_back(id1 - 1);
						tmpFace.vertices.push_back(id2 - 1);
						tmpFace.vertices.push_back(id3 - 1);

						// normal de cada vèrtex
						tmpFace.normals.clear();
						tmpFace.normals.push_back(nId1 - 1);
						tmpFace.normals.push_back(nId2 - 1);
						tmpFace.normals.push_back(nId3 - 1);

						// guardar el material de la cara
						tmpFace.material = darrerMaterial;
						this->faces.push_back(tmpFace);

						if (n == 8) isFaceQuad = true;
					}
					// If the face definition is of the form "f vert1/textcoord1/normal1 ..."
					else {
						int n = sscanf_s(buffer.c_str(), "%*s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &id1, &tId1, &nId1, &id2, &tId2, &nId2, &id3, &tId3, &nId3, &id4, &tId4, &nId4);
						if (n == 12) isFaceQuad = true;
						uvsIndices.push_back(tId1 - 1);
						uvsIndices.push_back(tId2 - 1);
						uvsIndices.push_back(tId3 - 1);

						struct Face tmpFace;

						// vèrtexos de la cara
						tmpFace.vertices.clear();
						tmpFace.vertices.push_back(id1 - 1);
						tmpFace.vertices.push_back(id2 - 1);
						tmpFace.vertices.push_back(id3 - 1);

						// normal de cada vèrtex
						tmpFace.normals.clear();
						tmpFace.normals.push_back(nId1 - 1);
						tmpFace.normals.push_back(nId2 - 1);
						tmpFace.normals.push_back(nId3 - 1);

						// guardar el material de la cara
						tmpFace.material = darrerMaterial;
						this->faces.push_back(tmpFace);

						if (isFaceQuad) uvsIndices.push_back(tId4 - 1);
					}
					normalsIndices.push_back(nId1 - 1);
					normalsIndices.push_back(nId2 - 1);
					normalsIndices.push_back(nId3 - 1);
					if (isFaceQuad) normalsIndices.push_back(nId4 - 1);
				}
			}
			verticesIndices.push_back(id1 - 1);
			verticesIndices.push_back(id2 - 1);
			verticesIndices.push_back(id3 - 1);
			if (isFaceQuad) verticesIndices.push_back((id4 - 1));
			isQuad.push_back(isFaceQuad);
		}
	}

	assert(!verticesIndices.empty());
	assert(normalsIndices.empty() || normalsIndices.size() == verticesIndices.size());
	assert(uvsIndices.empty() || uvsIndices.size() == verticesIndices.size());
	meshFile.close();

	// ---------- Merge the data that we have collected from the file ---------- //
	// Mesh data
	std::vector<std::vector<unsigned int> > meshIndices;
	std::vector<Vector3> meshNormals;
	if (!normals.empty()) meshNormals = std::vector<Vector3>(vertices.size(), Vector3(0, 0, 0));
	std::vector<Vector2<int>> meshUVs;
	if (!uvs.empty()) meshUVs = std::vector<Vector2<int>>(vertices.size(), Vector2<int>(0, 0));

	// We cannot load mesh with several parts for the moment
	unsigned int meshPart = 0;

	// Fill in the vertex indices
	// We also triangulate each quad face
	meshIndices.push_back(std::vector<unsigned int>());
	for (size_t i = 0, j = 0; i < verticesIndices.size(); j++) {

		// Get the current vertex IDs
		unsigned int i1 = verticesIndices[i];
		unsigned int i2 = verticesIndices[i + 1];
		unsigned int i3 = verticesIndices[i + 2];

		// Add the vertex normal
		if (!normalsIndices.empty() && !normals.empty()) {
			meshNormals[i1] = normals[normalsIndices[i]];
			meshNormals[i2] = normals[normalsIndices[i + 1]];
			meshNormals[i3] = normals[normalsIndices[i + 2]];
		}

		// Add the vertex UV texture coordinates
		if (!uvsIndices.empty() && !uvs.empty()) {
			meshUVs[i1] = uvs[uvsIndices[i]];
			meshUVs[i2] = uvs[uvsIndices[i + 1]];
			meshUVs[i3] = uvs[uvsIndices[i + 2]];
		}

		// If the current vertex not in a quad (it is part of a triangle)
		if (!isQuad[j]) {

			// Add the vertex indices
			meshIndices[meshPart].push_back(i1);
			meshIndices[meshPart].push_back(i2);
			meshIndices[meshPart].push_back(i3);

			i += 3;
		}
		else {  // If the current vertex is in a quad

			Vector3 v1 = vertices[i1];
			Vector3 v2 = vertices[i2];
			Vector3 v3 = vertices[i3];
			unsigned int i4 = verticesIndices[i + 3];
			Vector3 v4 = vertices[i4];

			Vector3 v13 = v3 - v1;
			Vector3 v12 = v2 - v1;
			Vector3 v14 = v4 - v1;

			float a1 = v13 * v12;
			float a2 = v13 *v14;
			if ((a1 >= 0 && a2 <= 0) || (a1 <= 0 && a2 >= 0)) {
				meshIndices[meshPart].push_back(i1);
				meshIndices[meshPart].push_back(i2);
				meshIndices[meshPart].push_back(i3);
				meshIndices[meshPart].push_back(i1);
				meshIndices[meshPart].push_back(i3);
				meshIndices[meshPart].push_back(i4);
			}
			else {
				meshIndices[meshPart].push_back(i1);
				meshIndices[meshPart].push_back(i2);
				meshIndices[meshPart].push_back(i4);
				meshIndices[meshPart].push_back(i2);
				meshIndices[meshPart].push_back(i3);
				meshIndices[meshPart].push_back(i4);
			}

			// Add the vertex normal
			if (!normalsIndices.empty() && !normals.empty()) {
				meshNormals[i4] = normals[normalsIndices[i]];
			}

			// Add the vertex UV texture coordinates
			if (!uvsIndices.empty() && !uvs.empty()) {
				meshUVs[i4] = uvs[uvsIndices[i]];
			}

			i += 4;
		}
	}

	assert(meshNormals.empty() || meshNormals.size() == vertices.size());
	assert(meshUVs.empty() || meshUVs.size() == vertices.size());

	// Set the data to the mesh
	mIndices = meshIndices;
	mVertices = vertices;
	mNormals = meshNormals;
	//meshToCreate.setUVs(meshUVs);

	std::cout << "Mesh loaded. Total cares: " << (meshIndices[0].size() / 3.0f) << std::endl;
}


void Object3D::draw() {
	/*for (int i = 0; i < this->mIndices.size(); i++) {
		std::vector<unsigned int> idx = this->mIndices[i];

		glBegin(GL_TRIANGLES);
		for (int j = 0; j < idx.size(); j++) {
			Vector3 v = this->mVertices[idx[j]];
			Vector3 n = this->mNormals[idx[j]];
			
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(v.x, v.y, v.z);
		}

		glEnd();
	}*/
	std::string ant;
	for (unsigned int i = 0; i < this->faces.size(); i++) {
		struct Face& tmp = this->faces[i];

		// establir el material si aquesta cara en té
		if (!tmp.material.empty()) {
			auto material = this->materials.find(tmp.material);
			if (material != this->materials.end()) {
				// existeix aquest material
				//std::cout << "Material " << material->first << std::endl;
				if (material->second.getNom() != ant) {
					material->second.setMaterial();
				}
				ant = material->second.getNom();
			}
			else {
				//std::cout << "Material no trobat " << tmp.material << std::endl;
			}
		}
		else {
			std::cout << "Cara sense material" << std::endl;
		}

		glBegin(GL_TRIANGLES);
		for (unsigned int j = 0; j < tmp.vertices.size(); j++) {
			Vector3& v = this->mVertices[tmp.vertices[j]];
			Vector3& n = this->mNormals[tmp.vertices[j]];

			glNormal3d(n.x, n.y, n.z);
			glVertex3d(v.x, v.y, v.z);
		}
		glEnd();
	}
}
