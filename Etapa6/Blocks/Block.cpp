#include "Block.h"
#include "../World.h"
#include <iostream>

Block::Block(World* world, Bloc id, Block* parent) {
	this->world = world;
	this->id = id;
	this->parent = parent;
}

Block::Block() {
	this->world = NULL;
	this->id = Bloc::RES;
	this->parent = 0;
}

void Block::draw() {
	bool visible[6] = { true, true, true, true, true, true };
	this->draw(visible); //Dibuixam el bloc sense wireframe
}

void Block::update(int delta) {

}

//NOTA: Els dibuixos est�tics (quan s'han de col�locar) d'estructures en moviment seran aqu�
/*
	Dibuixam el bloc segons el seu tipus
*/
void Block::draw(bool visible[6]) {
	glBindTexture(GL_TEXTURE_2D, 0); //Ens asseguram que no hi ha cap textura assignada
	float specularZero[4] = { 0,0,0,0 };
	float specularDef[4] = { 1,1,1,1 };
	switch (this->id) { //Dibuixam el que correspongui per cada bloc
	case Bloc::AIGUA: //Aigua
		glColor4f(0, 0, 1, 0.5f);
		//glDisable(GL_CULL_FACE);
		drawCub(visible);
		//glEnable(GL_CULL_FACE);
		break;
	case Bloc::CUB: //Cub vermell
		glColor3f(1, 0, 0);
		drawCub(visible);
		break;
	case Bloc::HERBA: //Herba
		glColor3f(0, 1, 0);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularZero); //No volem que brilli
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
	case Bloc::HERBAFULL: //Herba
		glColor3f(0, 1, 0);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularZero); //No volem que brilli
		glLineWidth(3.0f);
		glBegin(GL_LINES);
		glNormal3f(0, 1, 0);
		glVertex3d(0.5, 0.5, 0.5); glVertex3d(0.5, -0.5, 0.5);
		glVertex3d(-0.5, 0.5, 0.5); glVertex3d(-0.5, -0.5, 0.5);
		glVertex3d(0.5, 0.5, -0.5); glVertex3d(0.5, -0.5, -0.5);
		glVertex3d(-0.5, 0.5, -0.5); glVertex3d(-0.5, -0.5, -0.5);
		glVertex3d(0.0, 0.5, 0.0); glVertex3d(0.0, -0.5, 0.0);
		glVertex3d(0.5, 0.5, 0.0); glVertex3d(0.5, -0.5, 0.0);
		glVertex3d(0.0, 0.5, 0.5); glVertex3d(0.0, -0.5, 0.5);
		glVertex3d(-0.5, 0.5, 0.0); glVertex3d(-0.5, -0.5, 0.0);
		glVertex3d(0.0, 0.5, -0.5); glVertex3d(0.0, -0.5, -0.5);
		glVertex3d(0.25, 0.5, 0.25); glVertex3d(0.25, -0.5, 0.25);
		glVertex3d(-0.25, 0.5, 0.25); glVertex3d(-0.25, -0.5, 0.25);
		glVertex3d(0.25, 0.5, -0.25); glVertex3d(0.25, -0.5, -0.25);
		glVertex3d(-0.25, 0.5, -0.25); glVertex3d(-0.25, -0.5, -0.25);
		glVertex3d(0.25, 0.5, 0); glVertex3d(0.25, -0.5, 0);
		glVertex3d(0, 0.5, 0.25); glVertex3d(0, -0.5, 0.25);
		glVertex3d(-0.25, 0.5, 0); glVertex3d(-0.25, -0.5, 0);
		glVertex3d(0, 0.5, -0.25); glVertex3d(0, -0.5, -0.25);
		glEnd();
		glLineWidth(1.0f);
		break;
	case Bloc::TERRA: //Cub marr�
		glColor3f(0.5f, 0.35f, 0.05f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularZero); //No volem que brilli
		glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::TERRA));
		drawCub(visible);
		//ModelManager::drawModel(Model::CUB);
		break;
	case Bloc::VIDRE: //cub transparent/opac (vidre?)
		glColor4f(0, 0, 0, 1.0f);
		//glutWireCube(1.0f);
		//glutSolidCube(0.1f);
		glDepthMask(GL_FALSE);
		glColor4f(0.0f, 0.0f, 1.0f, 0.05f);
		drawCub(visible);
		glDepthMask(GL_TRUE);
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
	case Bloc::LLUMSOTIL: //Llum de s�til
		glDisable(GL_LIGHTING);
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		glVertex3d(0.5, 0.5, 0.5); glVertex3d(0.5, 0.5, -0.5); glVertex3d(-0.5, 0.5, -0.5); glVertex3d(-0.5, 0.5, 0.5);
		glColor3f(0, 0, 0);
		glLineWidth(2.0f);
		glEnd();
		glBegin(GL_LINE_LOOP);
		glVertex3d(0.5, 0.5, 0.5); glVertex3d(0.5, 0.5, -0.5); glVertex3d(-0.5, 0.5, -0.5); glVertex3d(-0.5, 0.5, 0.5); glVertex3d(0.5, 0.5, 0.5);
		glEnd();
		glEnable(GL_LIGHTING);
		break;
	case Bloc::LLUMTERRA: //Foco (llum de s�til per� de baix cap adalt)
		glDisable(GL_LIGHTING);
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		glVertex3d(0.5, -0.5, 0.5); glVertex3d(0.5, -0.5, -0.5); glVertex3d(-0.5, -0.5, -0.5); glVertex3d(-0.5, -0.5, 0.5);
		glColor3f(0, 0, 0);
		glLineWidth(2.0f);
		glEnd();
		glBegin(GL_LINE_LOOP);
		glVertex3d(0.5, -0.5, 0.5); glVertex3d(0.5, -0.5, -0.5); glVertex3d(-0.5, -0.5, -0.5); glVertex3d(-0.5, -0.5, 0.5); glVertex3d(0.5, -0.5, 0.5);
		glEnd();
		glEnable(GL_LIGHTING);
		break;
	case Bloc::MULTICOLOR: { //Cub multicolor
		float lado = 1.0f;

		GLfloat translacions[6][3] =
		{
			{0, -0.5f, 0}, //Abaix
			{0, 0.5f, 0}, //Adalt
			{-0.5f, 0 ,0}, //Esquerra
			{0.5f, 0 ,0}, //Dreta
			{0, 0, -0.5f}, //Darrera
			{0, 0, 0.5f} //Davant
		};
		GLfloat normals[6][3] =
		{
			{0, -1 ,0}, //Abaix
			{0, -1, 0}, //Adalt
			{0, -1, 0}, //Esquerra
			{0, -1, 0}, //Dreta
			{0, -1, 0}, //Darrera
			{0, -1, 0} //Davant
		};
		GLfloat rotacions[6][3] =
		{
			{0, 0, 0}, //Abaix
			{180, 0, 0}, //Adalt
			{0, 0 ,-90}, //Esquerra
			{0, 0 ,90}, //Dreta
			{0, -90, -90}, //Darrera
			{0, -90, 90}, //Davant
		};
		GLfloat colors[3][3][3] = 
		{
			{{0, 0, 0}, {0, 0, 1}, {0, 1, 0}},
			{{1, 0, 0}, {0, 1, 1}, {1, 1, 0}},
			{{1, 0, 1}, {1, 1, 1}, {0.5f, 0.5f, 0.5f}}
		};
		for (int i = 0; i < 6; i++) {
			glPushMatrix();
			glNormal3fv(normals[i]);
			glTranslatef(translacions[i][0], translacions[i][1], translacions[i][2]);
			glRotatef(rotacions[i][0], 1, 0, 0);
			glRotatef(rotacions[i][1], 0, 1, 0);
			glRotatef(rotacions[i][2], 0, 0, 1);
			glTranslatef(-0.5f, 0, -0.5f);
			for (float x = 0; x < 1; x += (1.0f / 3.0f)) {
				for (float z = 0; z < 1; z += (1.0f / 3.0f)) {
					glColor3fv(colors[int(x * 3)][int(z * 3)]);
					glBegin(GL_QUADS);
					glVertex3f(x, 0, z + (1.0f/3.0f));
					glVertex3f(x, 0, z); 
					glVertex3f(x + (1.0f / 3.0f), 0, z);
					glVertex3f(x + (1.0f / 3.0f), 0, z + (1.0f / 3.0f));
					glEnd();
				}
			}
			glPopMatrix();
		}
		break;
		}
	case Bloc::FUSTA:
		glColor3f(0.76f, 0.60f, 0.42f);
		//Aplicam la textura corresponent
		glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::FUSTA));
		drawCub(visible);
		//ModelManager::drawModel(Model::CUB);
		break;
	case Bloc::PEDRA:
		glColor3f(0.5f, 0.5f, 0.5f);
		glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::PEDRA));
		drawCub(visible);
		//ModelManager::drawModel(Model::CUB);
		break;
	case Bloc::NORIA: { //Noria (icona)
		glPushMatrix();
		float angle = toRad(0);
		glLineWidth(1.0f);
		glColor3f(0, 0, 0);
		for (int i = 0; i < 5; i++) {

			glPushMatrix();
			glBegin(GL_LINES); //Linia del centre al vag�
			glVertex3f(0, 0, 0);
			glVertex3f(0.5f * cosf(angle), 0.5f * sinf(angle), 0);
			glEnd();

			glPopMatrix();

			glPushMatrix();
			glBegin(GL_LINES); //Connexi� cap al pr�xim vag�
			glVertex3f(0.5f * cosf(angle), 0.5f * sinf(angle), 0);
			angle += toRad(360 / 5);
			glVertex3f(0.5f * cosf(angle), 0.5f * sinf(angle), 0);
			glEnd();

			glPopMatrix();

			//Suport d'enterra
			glColor3f(0, 0, 0);
			glLineWidth(3.0f);
			glBegin(GL_LINES); //Linia del centre al vag�
			glVertex3f(0, 0, 0);
			glVertex3f(1 / 5, -0.5f, 1 / 5);

			glVertex3f(0, 0, 0);
			glVertex3f(-1 / 5, -0.5f, 1 / 5);

			glVertex3f(0, 0, 0);
			glVertex3f(1 / 5, -0.5f, -1 / 5);

			glVertex3f(0, 0, 0);
			glVertex3f(-1 / 5, -0.5f, -1 / 5);
			glEnd();
			glLineWidth(1.0f);

		}
		glPopMatrix();
		break;
		}
	case Bloc::ALTAVEU:
		glColor3f(0.5f, 0.5f, 0.5f);
		glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::ALTAVEU));
		drawCub(visible);
		//ModelManager::drawModel(Model::CUB);
		break;
	case Bloc::MIRALL: //Nom�s icona, sense textura, quadrat
		glLineWidth(2.0f);
		glColor3f(0, 0, 0);

		glBegin(GL_LINES);
		glVertex3f(-0.5f, 0.5f, 0);
		glVertex3f(-0.5f, -0.5f, 0);

		glVertex3f(-0.5f, -0.5f, 0);
		glVertex3f(0.5f, -0.5f, 0);

		glVertex3f(0.5f, -0.5f, 0);
		glVertex3f(0.5f, 0.5f, 0);

		glVertex3f(-0.5f, 0.5f, 0);
		glVertex3f(0.5f, 0.5f, 0);
		glEnd();
		break;
	case Bloc::GRUA: //Nom�s icona, sense textura, quadrat
		glLineWidth(2.0f);
		glColor3f(0, 0, 0);

		glBegin(GL_LINES);
		glVertex3f(0.5f, 0.5f, 0);
		glVertex3f(0.5f, -0.5f, 0);

		glVertex3f(-0.3f, 0.4f, 0);
		glVertex3f(-0.3f, 0, 0);

		glVertex3f(-0.5f, 0.4f, 0);
		glVertex3f(0.5f, 0.4f, 0);
		glEnd();
		break;
	case Bloc::FUSTAARBRE:
		glColor3f(0.76f, 0.60f, 0.42f);
		glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::FUSTAARBRE));
		drawCub(visible);
		//ModelManager::drawModel(Model::CUB);
		break;
	case Bloc::FULLAARBRE:
		glColor3f(0, 0.5f, 0);
		glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::FULLAARBRE));
		drawCub(visible);
		//ModelManager::drawModel(Model::CUB);
		break;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularDef); //Restauram la llum specular

}

Block* Block::getParent() {
	return this->parent;
}

Bloc Block::getId() {
	return this->id;
}

void Block::setId(Bloc id) {
	this->id = id;
}

void Block::destroy(){
	if (parent != 0) {
		this->parent->destroy();
	}
};

void Block::interact() {
	if (parent != 0) {
		this->parent->interact();
	}
};

bool Block::isTransparent(Bloc tipus) {
	if (tipus == Bloc::RES || tipus == Bloc::AIRE || tipus == Bloc::HERBA || tipus == Bloc::VIDRE || tipus == Bloc::TORXA
		|| tipus == Bloc::HERBAFULL || tipus == Bloc::AIGUA) {
		return true;
	}
	return false;
}

bool Block::isSolid(Bloc tipus) {
	if (tipus != Bloc::RES && tipus != Bloc::AIRE && tipus != Bloc::HERBA && tipus != Bloc::TORXA && tipus != Bloc::HERBAFULL 
		&& tipus != Bloc::AIGUA) { //Exclude list
		return true;
	}
	return false;
}