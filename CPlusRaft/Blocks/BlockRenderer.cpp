#include "BlockRenderer.h"

BlockRenderer::BlockRenderer() {
	blockAtlas = new TextureAtlas(1, 8);
}

void BlockRenderer::drawBloc(Bloc id) {
	bool visible[6] = { true, true, true, true, true, true };
	drawBloc(id, visible);
}

void BlockRenderer::drawBloc(Bloc id, bool visible[6]) {
	//glBindTexture(GL_TEXTURE_2D, 0); //Ens asseguram que no hi ha cap textura assignada
	float specularZero[4] = { 0,0,0,0 };
	float specularDef[4] = { 1,1,1,1 };
	switch (id) { //Dibuixam el que correspongui per cada bloc
	case Bloc::AIGUA: //Aigua
		glColor4f(0, 0, 1, 0.5f);
		glDisable(GL_CULL_FACE);
		drawCub(visible);
		glEnable(GL_CULL_FACE);
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
	case Bloc::TERRA: //Cub marró
		glColor3f(0.5f, 0.35f, 0.05f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularZero); //No volem que brilli
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::TERRA));
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::BLOC));
		drawCub(visible, 6);
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
	case Bloc::LLUMSOTIL: //Llum de sòtil
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
	case Bloc::LLUMTERRA: //Foco (llum de sòtil però de baix cap adalt)
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
	//case Bloc::MULTICOLOR: { //Cub multicolor
	//	float lado = 1.0f;

	//	GLfloat translacions[6][3] =
	//	{
	//		{0, -0.5f, 0}, //Abaix
	//		{0, 0.5f, 0}, //Adalt
	//		{-0.5f, 0 ,0}, //Esquerra
	//		{0.5f, 0 ,0}, //Dreta
	//		{0, 0, -0.5f}, //Darrera
	//		{0, 0, 0.5f} //Davant
	//	};
	//	GLfloat normals[6][3] =
	//	{
	//		{0, -1 ,0}, //Abaix
	//		{0, -1, 0}, //Adalt
	//		{0, -1, 0}, //Esquerra
	//		{0, -1, 0}, //Dreta
	//		{0, -1, 0}, //Darrera
	//		{0, -1, 0} //Davant
	//	};
	//	GLfloat rotacions[6][3] =
	//	{
	//		{0, 0, 0}, //Abaix
	//		{180, 0, 0}, //Adalt
	//		{0, 0 ,-90}, //Esquerra
	//		{0, 0 ,90}, //Dreta
	//		{0, -90, -90}, //Darrera
	//		{0, -90, 90}, //Davant
	//	};
	//	GLfloat colors[3][3][3] =
	//	{
	//		{{0, 0, 0}, {0, 0, 1}, {0, 1, 0}},
	//		{{1, 0, 0}, {0, 1, 1}, {1, 1, 0}},
	//		{{1, 0, 1}, {1, 1, 1}, {0.5f, 0.5f, 0.5f}}
	//	};
	//	for (int i = 0; i < 6; i++) {
	//		glPushMatrix();
	//		glNormal3fv(normals[i]);
	//		glTranslatef(translacions[i][0], translacions[i][1], translacions[i][2]);
	//		glRotatef(rotacions[i][0], 1, 0, 0);
	//		glRotatef(rotacions[i][1], 0, 1, 0);
	//		glRotatef(rotacions[i][2], 0, 0, 1);
	//		glTranslatef(-0.5f, 0, -0.5f);
	//		for (float x = 0; x < 1; x += (1.0f / 3.0f)) {
	//			for (float z = 0; z < 1; z += (1.0f / 3.0f)) {
	//				glColor3fv(colors[int(x * 3)][int(z * 3)]);
	//				glBegin(GL_QUADS);
	//				glVertex3f(x, 0, z + (1.0f / 3.0f));
	//				glVertex3f(x, 0, z);
	//				glVertex3f(x + (1.0f / 3.0f), 0, z);
	//				glVertex3f(x + (1.0f / 3.0f), 0, z + (1.0f / 3.0f));
	//				glEnd();
	//			}
	//		}
	//		glPopMatrix();
	//	}
	//	break;
	//}
	case Bloc::FUSTA:
		glColor3f(0.76f, 0.60f, 0.42f);
		//Aplicam la textura corresponent
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::FUSTA));
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::BLOC));
		drawCub(visible, 3);
		//ModelManager::drawModel(Model::CUB);
		break;
	case Bloc::PEDRA:
		glColor3f(0.5f, 0.5f, 0.5f);
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::PEDRA));
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::BLOC));
		drawCub(visible, 5);
		//ModelManager::drawModel(Model::CUB);
		break;
	case Bloc::ALTAVEU:
		glColor3f(0.5f, 0.5f, 0.5f);
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::ALTAVEU));
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::BLOC));
		drawCub(visible, 0);
		//ModelManager::drawModel(Model::CUB);
		break;
	case Bloc::FUSTAARBRE:
		glColor3f(0.76f, 0.60f, 0.42f);
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::FUSTAARBRE));
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::BLOC));
		drawCub(visible, 4);
		//ModelManager::drawModel(Model::CUB);
		break;
	case Bloc::FULLAARBRE:
		glColor3f(0, 0.5f, 0);
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::FULLAARBRE));
		//glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::BLOC));
		drawCub(visible, 2);
		//ModelManager::drawModel(Model::CUB);
		break;
	}
	//glBindTexture(GL_TEXTURE_2D, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularDef); //Restauram la llum specular

}

float* BlockRenderer::getTexCoords(Bloc tipus) {
	int texNum = -1;
	switch (tipus) {
	case Bloc::TERRA: //Cub marró
		texNum = 6;
		break;
	case Bloc::FUSTA:
		texNum = 3;
		break;
	case Bloc::PEDRA:
		texNum = 5;
		break;
	case Bloc::ALTAVEU:
		texNum = 0;
		break;
	case Bloc::FUSTAARBRE:
		texNum = 4;
		break;
	case Bloc::FULLAARBRE:
		texNum = 2;
		break;
	}
	return blockAtlas->getCoords(texNum);
}

void BlockRenderer::drawCub(bool cares[6], int texNum) {
	float xt = 0, xb = 0, yt = 0, yb = 0;

	float* tCoords = blockAtlas->getCoords(texNum);

	if (texNum != -1) {
		xb = tCoords[0];
		yb = tCoords[1];
		xt = tCoords[2];
		yt = tCoords[3];
	}

	GLfloat n[6][3] =
	{
	  {-1.0, 0.0, 0.0}, //Esquerra
	  {0.0, 1.0, 0.0}, //Amunt
	  {1.0, 0.0, 0.0}, //Dreta
	  {0.0, -1.0, 0.0}, //Abaix
	  {0.0, 0.0, 1.0}, //Davant
	  {0.0, 0.0, -1.0} //Darrera
	};
	GLfloat text[6][4][2] =
	{
		{{-xt,yt}, {xb,yt}, {xb,yb}, {-xt, yb}}, //Esquerra OK
		{{-xt,yb}, {-xt,yt}, {xb,yt}, {xb,yb}}, //Damunt OK
		{{xt, yb}, {xb,yb}, {xb,yt}, {xt,yt}}, //Dreta OK
		{{xt,yt}, {xt,yb}, {xb,yb}, {xb,yt}}, //Abaix OK
		{{xt, yt}, {xt,yb}, {xb,yb}, {xb,yt}}, //Davant OK
		{{-xt,yb}, {-xt,yt}, {xb,yt}, {xb,yb}} //Darrera OK
	};


	GLint faces[6][4] =
	{
	  {0, 1, 2, 3}, //Esquerra
	  {3, 2, 6, 7}, //Damunt
	  {7, 6, 5, 4}, //Dreta
	  {4, 5, 1, 0}, //Abaix
	  {5, 6, 2, 1}, //Davant
	  {7, 4, 0, 3} //Darrera
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -0.5f;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = 0.5f;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -0.5f;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = 0.5f;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -0.5f;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = 0.5f;

	glBegin(GL_QUADS);
	for (i = 5; i >= 0; i--) { //6 cares
		if (cares[i]) {
			glNormal3fv(&n[i][0]);
			glTexCoord2fv(text[i][0]);
			glVertex3fv(&v[faces[i][0]][0]); //Top left
			glTexCoord2fv(text[i][1]);
			glVertex3fv(&v[faces[i][1]][0]); //Bottom left
			glTexCoord2fv(text[i][2]);
			glVertex3fv(&v[faces[i][2]][0]); //Bottom right
			glTexCoord2fv(text[i][3]);
			glVertex3fv(&v[faces[i][3]][0]); //Top right
		}
	}
	glEnd();
}

float* BlockRenderer::getColor(Bloc tipus) {
	float color[4] = { 0, 0, 0, 1 };
	switch (tipus) { //Dibuixam el que correspongui per cada bloc
	case Bloc::AIGUA: //Aigua
		color[0] = 0; color[1] = 0; color[2] = 1; color[3] = 0.5f;
		break;
	case Bloc::CUB: //Cub vermell
		color[0] = 1; color[1] = 0; color[2] = 0; color[3] = 1;
		break;
	case Bloc::HERBA: //Herba
		color[0] = 0; color[1] = 1; color[2] = 0; color[3] = 1;
		break;
	case Bloc::HERBAFULL: //Herba
		color[0] = 0; color[1] = 1; color[2] = 0; color[3] = 1;
		break;
	case Bloc::TERRA: //Cub marró
		color[0] = 0.5f; color[1] = 0.35f; color[2] = 0.05f; color[3] = 1;
		break;
	case Bloc::VIDRE: //cub transparent/opac (vidre?)
		color[0] = 0; color[1] = 0; color[2] = 0; color[3] = 1;
		break;
	case Bloc::TORXA: //torxa
		color[0] = 0.5f; color[1] = 0.35f; color[2] = 0.05f; color[3] = 1;
		break;
	case Bloc::LLUMSOTIL: //Llum de sòtil
		color[0] = 0; color[1] = 0; color[2] = 0; color[3] = 1;
		break;
	case Bloc::LLUMTERRA: //Foco (llum de sòtil però de baix cap adalt)
		color[0] = 0; color[1] = 0; color[2] = 0; color[3] = 1;
		break;
	case Bloc::FUSTA:
		color[0] = 0.76f; color[1] = 0.6f; color[2] = 0.42f; color[3] = 1;
		break;
	case Bloc::PEDRA:
		color[0] = 0.5f; color[1] = 0.5f; color[2] = 0.5f; color[3] = 1;
		break;
	case Bloc::ALTAVEU:
		color[0] = 0.5f; color[1] = 0.5f; color[2] = 0.5f; color[3] = 1;
		break;
	case Bloc::FUSTAARBRE:
		color[0] = 0.76f; color[1] = 0.6f; color[2] = 0.42f; color[3] = 1;
		break;
	case Bloc::FULLAARBRE:
		color[0] = 0; color[1] = 0.5f; color[2] = 0; color[3] = 1;
		break;
	}
	return color;
}

void BlockRenderer::drawCub(bool cares[6]) {
	this->drawCub(cares, -1);
}