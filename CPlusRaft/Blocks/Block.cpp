#include "Block.h"
#include "../World.h"
#include "../Utils.h"
#include "LightBlock.h"
#include "Jukebox.h"

bool Block::marching = false;
TextureAtlas blockAtlas = TextureAtlas(8,3);

//Les texCoords és retornen en ordre xb, yb, xt, yt i el color en RGBA
bool Block::getBlockInfo(Bloc id, std::array<float, 4>& texCoords, std::array<unsigned char, 4> &color) {
	int texNum = -1; //Per defecte sense textura

	switch (id) {
	case Bloc::CUB: //Cub vermell
		//color = { 1, 0, 0, 1 };
		color = { 255, 0, 0, 255 };
		break;
	case Bloc::TERRA: //Cub marró
		texNum = 6;
		//color = { 0.5f, 0.35f, 0.05f, 1 };
		color = { 127, 90, 12, 255 };
		break;
	case Bloc::VIDRE: //cub transparent/opac (vidre?)
		//color = { 0, 0, 0, 1 };
		texNum = 11;
		color = { 0, 0, 0, 51 };
		break;
	case Bloc::FUSTA:
		texNum = 3;
		//color = { 0.76f, 0.6f, 0.42f, 1 };
		color = { 190, 160, 100, 255 };
		break;
	case Bloc::PEDRAW:
		texNum = 5;
		//color = { 0.5f, 0.5f, 0.5f, 1 };
		color = { 127, 127, 127, 255 };
		break;
	case Bloc::PEDRA:
		texNum = 15;
		//color = { 0.5f, 0.5f, 0.5f, 1 };
		color = { 127, 127, 127, 255 };
		break;
	case Bloc::PEDRAS:
		texNum = 16;
		//color = { 0.5f, 0.5f, 0.5f, 1 };
		color = { 127, 127, 127, 255 };
		break;
	case Bloc::FUSTAARBRE:
		texNum = 4;
		//color = { 0.76f, 0.6f, 0.42f, 1 };
		color = { 190, 160, 100, 255 };
		break;
	case Bloc::FULLAARBRE:
		texNum = 2;
		//color = { 0, 0.5f, 0, 1 };
		color = { 0, 127, 0, 255 };
		break;
	case Bloc::NEU: case Bloc::NEUSUP:
		texNum = 1;
		//color = { 1,1,1,1 };
		color = { 255, 255, 255, 255 };
		break;
	case Bloc::GEL:
		texNum = 7;
		//color = { 0,0,1,0.8f };
		color = { 0, 0, 255, 210 };
		break;
	case Bloc::ARENA:
		//color = { 0.929f, 0.788f, 0.686f, 1 };
		color = { 240, 200, 180, 255 };
		texNum = 18;
		break;
	case Bloc::OR:
		//color = { 1, 0.843f, 0, 1 };
		color = { 255, 210, 0, 255 };
		texNum = 12;
		break;
	case Bloc::FANG:
		//color = { 0.5f, 0.35f, 0.05f, 1 };
		color = { 127, 80, 12, 255 };
		texNum = 8;
		break;
	case Bloc::FERRO:
		//color = { 0.5f, 0.5f, 0.5f, 1 };
		color = { 127, 127, 127, 255 };
		texNum = 9;
		break;
	case Bloc::GRAVILLA:
		//color = { 0.5f, 0.5f, 0.5f, 1 };
		color = { 127, 127, 127, 255 };
		texNum = 14;
		break;
	case Bloc::CARBO:
		//color = { 0.21f, 0.271f, 0.31f, 1 };
		color = { 51, 65, 75, 255 };
		texNum = 10;
		break;
	case Bloc::ALTAVEU: //Cub vermell
		//color = { 0.5f, 0.5f, 0.5f, 1 };
		color = { 127, 127, 127, 255 };
		texNum = 0;
		break;
	case Bloc::AIGUA: //Aigua
		//color = { 0, 0, 1, 0.7f };
		color = { 0, 0, 255, 185 };
		break;
	default:
		texCoords[0] = 0; texCoords[1] = 1; texCoords[0] = 2; texCoords[0] = 3;
		//color = { 1, 1, 1, 1 };
		color = { 255, 255, 255, 255 };
		return false; //Tipus invàlid
		break;
	}

	TextureManager::getBlockTexCoords(texNum, texCoords);
	return true;
}

Block::Block(Bloc id) {
	//this->world = world;
	this->id = id;
	//this->parent = parent;
}

Block::Block() {
	//this->world = NULL;
	this->id = Bloc::RES;
	//this->parent = 0;
}

void Block::update(float delta, World* world) {

}

Bloc Block::getId() {
	return this->id;
}

void Block::setId(Bloc id) {
	this->id = id;
}

void Block::destroy(World* world){

};

void Block::interact(World* world) {

};

void Block::draw(Bloc id, ChunkMesh* cM, Vector3<int> relPos, bool visible[6]) {
	switch (id) {
	case Bloc::HERBA: {
		GLfloat vert[17][2][3] = {
			{ {0.5, 0.5, 0.5}, {0.5, -0.5, 0.5} },
			{ {-0.5, 0.5, 0.5}, {-0.5, -0.5, 0.5}},
			{ {0.5, 0.5, -0.5}, {0.5, -0.5, -0.5} },
			{ {-0.5, 0.5, -0.5}, { -0.5, -0.5, -0.5}},
			{ {0.0, 0.5, 0.0}, { 0.0, -0.5, 0.0}},
			{ {0.5, 0.5, 0.0}, { 0.5, -0.5, 0.0}},
			{ {0.0, 0.5, 0.5}, { 0.0, -0.5, 0.5}},
			{ {-0.5, 0.5, 0.0}, { -0.5, -0.5, 0.0}},
			{ {0.0, 0.5, -0.5}, { 0.0, -0.5, -0.5}},
			{ {0.25, 0.5, 0.25}, { 0.25, -0.5, 0.25}},
			{ {-0.25, 0.5, 0.25}, { -0.25, -0.5, 0.25}},
			{ {0.25, 0.5, -0.25}, { 0.25, -0.5, -0.25}},
			{ {-0.25, 0.5, -0.25}, { -0.25, -0.5, -0.25}},
			{ {0.25, 0.5, 0}, { 0.25, -0.5, 0}},
			{ {0, 0.5, 0.25}, { 0, -0.5, 0.25}},
			{ {-0.25, 0.5, 0}, { -0.25, -0.5, 0}},
			{ {0, 0.5, -0.25}, { 0, -0.5, -0.25}}
		};

		int texNum = -1; //Per defecte sense textura
		unsigned char color[4] = { 0, 255, 0, 255 }; //Verd

		//float* texCoords = TextureManager::getTexCoords(texNum);
		float xb = 0, yb = 0, xt = 0, yt = 0;
		//xb = texCoords[0]; yb = texCoords[1]; xt = texCoords[2]; yt = texCoords[3];

		for (int i = 0; i < 17; i++) {
			for (int j = 0; j < 2; j++) {
				unsigned short normal[3] = { 0,1,0 };
				unsigned short text[2] = { 0,0 };
				unsigned short vPos[3] = { toHFloat(vert[i][j][0] + relPos.x), toHFloat(vert[i][j][1] + relPos.y), toHFloat(vert[i][j][2] + relPos.z) };
				cM->addVertexO(vPos, normal, color, text, Primitiva::LINIA);
			}
		}
		break;
	}
	case Bloc::TORXA: {
		GLfloat vert[2][2][3] = {
		{ {0, 0, 0}, {0, -0.5f, 0} },
		{ {0, 0.1f, 0}, {0, 0, 0}},
		};

		int texNum = -1; //Per defecte sense textura
		unsigned char color[2][4] = { {139,69,19, 255}, { 255, 255, 0, 255 } }; //Verd

		//float* texCoords = TextureManager::getTexCoords(texNum);
		float xb = 0, yb = 0, xt = 0, yt = 0;
		//xb = texCoords[0]; yb = texCoords[1]; xt = texCoords[2]; yt = texCoords[3];

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				unsigned short normal[3] = { 0,1,0 };
				unsigned short text[2] = { 0,0 };
				unsigned short vPos[3] = { toHFloat(vert[i][j][0] + relPos.x), toHFloat(vert[i][j][1] + relPos.y), toHFloat(vert[i][j][2] + relPos.z) };
				cM->addVertexO(vPos, normal, color[i], text, Primitiva::LINIA);
			}
		}
		break;
	}
	default:
		Block::drawBlock(id, cM, relPos, visible);
		break;
	}
}

bool Block::isTransparent(Bloc tipus) {
	return (tipus == Bloc::VIDRE || tipus == Bloc::AIGUA || tipus == Bloc::GEL);
}

bool Block::canSeeThrough(Bloc tipus) {
	return (Block::isTransparent(tipus) || tipus == Bloc::TORXA || tipus == Bloc::HERBA || tipus == Bloc::RES || tipus == Bloc::NEUSUP);
}

bool Block::isSolid(Bloc tipus) {
	return (tipus != Bloc::RES && tipus != Bloc::HERBA && tipus != Bloc::TORXA && tipus != Bloc::AIGUA && tipus != Bloc::NEUSUP);
}

bool Block::isInteractable(Bloc tipus) {
	return (Block::isSolid(tipus) || tipus == Bloc::HERBA || tipus == Bloc::NEUSUP || tipus == Bloc::TORXA);
}

//Indica si a un bloc se li pot aplicar marching cubes. Ex: Als transparents i solids (gel) no s'ha de permetre
bool Block::isMarcheable(Bloc tipus) {
	return (isSolid(tipus) && !canSeeThrough(tipus));
}

bool Block::isCube(Bloc tipus) {
	return (tipus != Bloc::HERBA && tipus != Bloc::TORXA);
}

bool Block::isEspecial(Bloc tipus) {
	return (tipus == Bloc::TORXA || tipus == Bloc::ALTAVEU);
}

Block* Block::creaEspecial(Bloc tipus, Vector3<int> pos, World* world) {
	switch (tipus) {
	case Bloc::TORXA:
		return (Block*)(new LightBlock(world, tipus, pos));
	case Bloc::ALTAVEU:
		return (Block*)(new Jukebox(pos));
	default:
		return nullptr;
	}
}

bool Block::drawIcon(Bloc id) {
	std::array<unsigned char, 4> color;
	std::array<float, 4> texCoords;
	Block::getBlockInfo(id, texCoords, color);
	if (Block::isCube(id)) {
		glColor4ubv(reinterpret_cast<GLubyte*>(color.data()));

		glFrontFace(GL_CCW);

		float xt = 0, xb = 0, yt = 0, yb = 0;

		xb = texCoords[0];
		yb = texCoords[1];
		xt = texCoords[2];
		yt = texCoords[3];

		GLfloat n[6][3] =
		{
		  {-1.0, 0.0, 0.0}, //Esquerra
		  {0.0, 1.0, 0.0}, //Amunt
		  {1.0, 0.0, 0.0}, //Dreta
		  {0.0, -1.0, 0.0}, //Abaix
		  {0.0, 0.0, 1.0}, //Davant
		  {0.0, 0.0, -1.0} //Darrera
		};
		GLfloat text[4][2] ={ {xt, yt}, {xt,yb}, {xb,yb}, {xb,yt} }; //Davant OK


		GLint face[4] = { 5, 6, 2, 1 }; //Davant
		GLfloat v[8][3];

		v[0][0] = v[1][0] = v[2][0] = v[3][0] = -0.5f;
		v[4][0] = v[5][0] = v[6][0] = v[7][0] = 0.5f;
		v[0][1] = v[1][1] = v[4][1] = v[5][1] = -0.5f;
		v[2][1] = v[3][1] = v[6][1] = v[7][1] = 0.5f;
		v[0][2] = v[3][2] = v[4][2] = v[7][2] = -0.5f;
		v[1][2] = v[2][2] = v[5][2] = v[6][2] = 0.5f;

		glBegin(GL_QUADS);
		glNormal3fv(&n[4][0]);
		glTexCoord2fv(text[0]);
		glVertex3fv(&v[face[0]][0]); //Top left
		glTexCoord2fv(text[1]);
		glVertex3fv(&v[face[1]][0]); //Bottom left
		glTexCoord2fv(text[2]);
		glVertex3fv(&v[face[2]][0]); //Bottom right
		glTexCoord2fv(text[3]);
		glVertex3fv(&v[face[3]][0]); //Top right
		glEnd();
	}
	else {
		switch (id) { //Dibuixam el que correspongui per cada bloc
		case Bloc::HERBA: //Herba
			glColor4f(0, 1, 0,1);
			glLineWidth(3.0f);
			glBegin(GL_LINES);
			glNormal3f(0, 1, 0);
			glVertex3d(0.5, 0.3, 0.5); glVertex3d(0.5, -0.5, 0.5);
			glVertex3d(-0.5, 0.5, 0.5); glVertex3d(-0.5, -0.5, 0.5);
			glVertex3d(0.5, 0.1, -0.5); glVertex3d(0.5, -0.5, -0.5);
			glVertex3d(-0.5, 0.4, -0.5); glVertex3d(-0.5, -0.5, -0.5);
			glVertex3d(0.0, 0.5, 0.0); glVertex3d(0.0, -0.5, 0.0);
			glVertex3d(0.5, 0.2, 0.0); glVertex3d(0.5, -0.5, 0.0);
			glVertex3d(0.0, 0.4, 0.5); glVertex3d(0.0, -0.5, 0.5);
			glVertex3d(-0.5, 0.2, 0.0); glVertex3d(-0.5, -0.5, 0.0);
			glVertex3d(0.0, 0.4, -0.5); glVertex3d(0.0, -0.5, -0.5);
			glVertex3d(0.25, 0.3, 0.25); glVertex3d(0.25, -0.5, 0.25);
			glVertex3d(-0.25, 0.5, 0.25); glVertex3d(-0.25, -0.5, 0.25);
			glVertex3d(0.25, 0.1, -0.25); glVertex3d(0.25, -0.5, -0.25);
			glVertex3d(-0.25, 0.4, -0.25); glVertex3d(-0.25, -0.5, -0.25);
			glVertex3d(0.25, 0.3, 0); glVertex3d(0.25, -0.5, 0);
			glVertex3d(0, 0.5, 0.25); glVertex3d(0, -0.5, 0.25);
			glVertex3d(-0.25, 0.1, 0); glVertex3d(-0.25, -0.5, 0);
			glVertex3d(0, 0.4, -0.25); glVertex3d(0, -0.5, -0.25);
			glEnd();
			glLineWidth(1.0f);
			break;
		case Bloc::TORXA: //torxa
			glDisable(GL_LIGHTING);
			glLineWidth(3.0f);
			glColor3f(0.5f, 0.35f, 0.05f);
			glBegin(GL_LINES);
			glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, -0.5, 0.0); //Cos
			glEnd();
			glColor3f(1, 1, 0);
			glBegin(GL_LINES);
			glVertex3d(0.0, 0.2, 0.0); glVertex3d(0.0, 0.1, 0.0); //Punta
			glEnd();
			glLineWidth(1.0f);
			glEnable(GL_LIGHTING);
			break;
		}
	}
	return true;
}

void Block::drawBlock(Bloc id, ChunkMesh* cM, Vector3<int> relPos, bool visible[6]) {
	float height = 1.0f;
	if (id == Bloc::NEUSUP) {
		height = 0.2f;
	}
	if (height < 1.0f) { //En aquest cas, la cara d'adalt sempre serà visible
		visible[static_cast<int>(Cara::ADALT)] = true;

	}
	float vert[6][4][3] = {
	{{-.5f, -.5f + height, .5f},  {-.5f, -.5f + height,-.5f},  {-.5f,-.5f,-.5f}, {-.5f,-.5f, .5f}}, // v1,v6,v7,v2 (left)
	{{.5f, -.5f + height, .5f},   {.5f, -.5f + height,-.5f},  {-.5f, -.5f + height,-.5f}, {-.5f, -.5f + height, .5f}}, // v0,v5,v6,v1 (top)
	{{.5f, -.5f + height, .5f},   {.5f,-.5f, .5f},   {.5f,-.5f,-.5f},  {.5f, -.5f + height,-.5f}}, // v0,v3,v4,v5 (right)
	{{-.5f,-.5f,-.5f},   {.5f,-.5f,-.5f},   {.5f,-.5f, .5f}, {-.5f,-.5f, .5f}}, // v7,v4,v3,v2 (bottom)
	{{.5f, -.5f + height, .5f},  {-.5f, -.5f + height, .5f},  {-.5f,-.5f, .5f},  {.5f,-.5f, .5f}}, // v0,v1,v2,v3 (front)
	{{.5f,-.5f,-.5f},  {-.5f,-.5f,-.5f},  {-.5f, -.5f + height,-.5f},  {.5f, -.5f + height,-.5f}}  // v4,v7,v6,v5 (back)
	};

	// normal array
	static float normals[6][4][3] = {
		{{-1.0f, 0, 0},  {-1.0f, 0, 0},  {-1.0f, 0, 0},  {-1.0f, 0, 0}},  // v1,v6,v7,v2 (left)
		{{0, 1.0f, 0},   {0, 1.0f, 0},   {0, 1.0f, 0},   {0, 1.0f, 0}},  // v0,v5,v6,v1 (top)
		{{1.0f, 0, 0},   {1.0f, 0, 0},   {1.0f, 0, 0},   {1.0f, 0, 0}},  // v0,v3,v4,v5 (right)
		{{0,-1.0f, 0},   {0,-1.0f, 0},   {0,-1.0f, 0},   {0,-1.0f, 0}},  // v7,v4,v3,v2 (bottom)
		{{0, 0, 1.0f},   {0, 0, 1.0f},   {0, 0, 1.0f},   {0, 0, 1.0f}},  // v0,v1,v2,v3 (front)
		{{0, 0,-1.0f},   {0, 0,-1.0f},  {0, 0,-1.0f},   {0, 0,-1.0f}}   // v4,v7,v6,v5 (back)
	};

	std::array<unsigned char, 4> color; //RGBA, Abstracció classe Color?
	std::array<float, 4> tCoords;
	Block::getBlockInfo(id, tCoords, color);
	float xb = 0, yb = 0, xt = 0, yt = 0;
	xb = tCoords[0]; yb = tCoords[1]; xt = tCoords[2]; yt = tCoords[3];
	float text[6][4][2] =
	{
		{{xt, yb}, {xb,yb}, {xb,yt}, {xt,yt}}, //Esquerra OK
		{{xt, yt}, {xt,yb}, {xb,yb}, {xb,yt}}, //Damunt OK
		{{xb, yb}, {xb,yt}, {xt,yt}, {xt, yb}}, //Dreta OK
		{{xb, yt}, {xt,yt}, {xt,yb}, {xb,yb}}, //Abaix OK
		{{xt, yb}, {xb,yb}, {xb,yt}, {xt,yt}}, //Davant OK
		{{xb,yt}, {xt, yt}, {xt,yb}, {xb,yb}}, //Darrera Ok
	};

	for (int i = 0; i < 6; i++) {
		if (visible[i]) {
			for (int j = 0; j < 4; j++) {
				unsigned short normalsN[3] = { toHFloat(normals[i][j][0]), toHFloat(normals[i][j][1]), toHFloat(normals[i][j][2]) };
				//printf("norm %f %f %f\n", toFloat(normalsN[0]), toFloat(normalsN[1]), toFloat(normalsN[2]));
				unsigned short textN[2] = { toHFloat(text[i][j][0]), toHFloat(text[i][j][1]) };
				unsigned short vPos[3] = { toHFloat(vert[i][j][0] + float(relPos.x)), toHFloat(vert[i][j][1] + float(relPos.y)), toHFloat(vert[i][j][2] + float(relPos.z)) };
				//vPos[0] += relPos.x; vPos[1] += relPos.y; vPos[2] += relPos.z;
				if (Block::isTransparent(id)) {
					cM->addVertexT(vPos, normalsN, color.data(), textN);
				}
				else {
					cM->addVertexO(vPos, normalsN, color.data(), textN, Primitiva::QUAD);
				}
			}
			if (id == Bloc::AIGUA) { //Els líquids s'han de veure des d'abaix també
				for (int j = 3; j >= 0; j--) {
					unsigned short normalsN[3] = { toHFloat(normals[i][j][0]), toHFloat(normals[i][j][1]), toHFloat(normals[i][j][2]) };
					unsigned short textN[2] = { toHFloat(text[i][j][0]), toHFloat(text[i][j][1]) };
					unsigned short vPos[3] = { toHFloat(vert[i][j][0] + float(relPos.x)), toHFloat(vert[i][j][1] + float(relPos.y)), toHFloat(vert[i][j][2] + float(relPos.z)) };
					cM->addVertexT(vPos, normalsN, color.data(), textN);
				}
			}
		}
	}

}

void Block::drawMarching(Bloc id, ChunkMesh* cM, Vector3<int> relPos, Chunk* ch) {

	static Vector3<int> toCheck[8] = { Vector3(0, 0, 0), Vector3(1, 0, 0), Vector3(1, 1, 0), Vector3(0, 1, 0),
						Vector3(0, 0, 1), Vector3(1, 0, 1), Vector3(1, 1, 1), Vector3(0, 1, 1) };

	std::array<float, 8> cube = {};
	Bloc neighbors[8] = { Bloc::RES, Bloc::RES, Bloc::RES, Bloc::RES, Bloc::RES, Bloc::RES, Bloc::RES, Bloc::RES };
	for (int i = 0; i < 8; i++) {
		//cube[i] = cnk->den[pos.x][pos.y][pos.z];
		Vector3<int> bpos = ch->getPos() * CHUNKSIZE + relPos + toCheck[i];
		Bloc bt = ch->getBlockWorld(bpos);
		if (!Block::isMarcheable(bt)) {
			cube[i] = 1;
		}
		neighbors[i] = bt;
	}

	std::vector< Vector3<float> > toDraw;
	std::vector< Vector3<float> > normals;
	MarchingCubes::apply(1, cube, toDraw, normals);

	//Decidim el tipus de bloc que ha de ser
	Bloc bt;
	if (id == Bloc::RES) {
		int freq[NBLOCS] = {};
		for (int i = 0; i < 8; i++) {
			if (!Block::isMarcheable(neighbors[i])) {
				continue;
			}
			freq[static_cast<int>(neighbors[i])]++;
		}
		int max = 0;
		int maxi = -1;
		for (int i = 0; i < NBLOCS; i++) {
			if (freq[i] > max) {
				maxi = i;
				max = freq[i];
			}
		}
		bt = static_cast<Bloc>(maxi); //Frequència màxima
	}
	else {
		bt = id;
	}

	std::array<unsigned char, 4> color; //RGBA, Abstracció classe Color?
	std::array<float, 4> tCoords;
	Block::getBlockInfo(bt, tCoords, color);
	float xb = tCoords[0], yb = tCoords[1], xt = tCoords[2], yt = tCoords[3];
	float text[4][2]{
		{xb, yb},
		{xb, yt},
		{xt, yb},
		{xt, yt}
	};

	std::vector< Vector3<float> >::iterator it;
	std::vector< Vector3<float> >::iterator it2;
	it2 = normals.begin();
	int i = 0;
	for (it = toDraw.begin(); it != toDraw.end(); it++) {

		Vector3 normalV = *it2;
		unsigned short normal[3] = { toHFloat(-normalV.x), toHFloat(-normalV.y), toHFloat(-normalV.z) };
		Vector3 pos = *it;
		unsigned short vPos[3] = { toHFloat(pos.x + relPos.x - 0.5f), toHFloat(pos.y + relPos.y - 0.5f), toHFloat(pos.z + relPos.z -0.5f) }; //-0.5f per compensar
		unsigned short textN[2] = { toHFloat(text[i][0]), toHFloat(text[i][1]) };
		cM->addVertexO(vPos, normal, color.data(), textN, Primitiva::TRIANGLE);
		i++;
		if (i == 3) {
			i = 0;
			it2++;
		}
	}
}

void Block::setMCEnabled(bool set) {
	Block::marching = set;
}

bool Block::getMCEnabled() {
	return Block::marching;
}