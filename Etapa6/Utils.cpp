#include "Utils.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <cmath>

//Codi de: https://stackoverflow.com/questions/36949957/loading-a-wav-file-for-openal
bool isBigEndian()
{
	int a = 1;
	return !((char*)&a)[0];
}

int convertToInt(char* buffer, int len)
{
	int a = 0;
	if (!isBigEndian())
		for (int i = 0; i < len; i++)
			((char*)&a)[i] = buffer[i];
	else
		for (int i = 0; i < len; i++)
			((char*)&a)[3 - i] = buffer[i];
	return a;
}

char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size)
{
	char buffer[4];
	std::ifstream in(fn, std::ios::binary);
	in.read(buffer, 4);
	if (strncmp(buffer, "RIFF", 4) != 0)
	{
		std::cout << "this is not a valid WAVE file" << std::endl;
		return NULL;
	}
	in.read(buffer, 4);
	in.read(buffer, 4);      //WAVE
	in.read(buffer, 4);      //fmt
	in.read(buffer, 4);      //16
	in.read(buffer, 2);      //1
	in.read(buffer, 2);
	chan = convertToInt(buffer, 2);
	in.read(buffer, 4);
	samplerate = convertToInt(buffer, 4);
	in.read(buffer, 4);
	in.read(buffer, 2);
	in.read(buffer, 2);
	bps = convertToInt(buffer, 2);
	in.read(buffer, 4);      //data
	in.read(buffer, 4);
	size = convertToInt(buffer, 4);
	char* data = new char[size];
	in.read(data, size);
	return data;
}

//Dibuixa un paralepipede d'una mida determinada. generant les coordenades de textures corresponents
void draw3dRect(float midaX, float midaY, float midaZ) {
    static GLfloat n[6][3] =
    {
      {-1.0, 0.0, 0.0}, //Esquerra
      {0.0, 1.0, 0.0}, //Amunt
      {1.0, 0.0, 0.0}, //Dreta
      {0.0, -1.0, 0.0}, //Abaix
      {0.0, 0.0, 1.0}, //Davant
      {0.0, 0.0, -1.0} //Darrera
    };
	static GLfloat text[6][4][2] =
	{
		{{-1,1}, {0,1}, {0,0}, {-1, 0}}, //Esquerra
		{{-1,-1}, {-1,0}, {0,0}, {0,-1}}, //Damunt
		{{1,-1}, {0,-1}, {0,0}, {1, 0}}, //Dreta
		{{1,1}, {1,0}, {0,0}, {0,1}}, //Abaix
		{{1,1}, {1,0}, {0,0}, {0,1}}, //Davant
		{{-1,-1}, {-1,0}, {0,0}, {0,-1}} //Darrera
	};
    static GLint faces[6][4] =
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

	if (midaX == 1 && midaY == 1 && midaZ == 1) {
		for (i = 5; i >= 0; i--) { //6 cares
			glBegin(GL_QUADS);
			glNormal3fv(&n[i][0]);
			glTexCoord2fv(text[i][0]);
			glVertex3fv(&v[faces[i][0]][0]); //Top left
			glTexCoord2fv(text[i][1]);
			glVertex3fv(&v[faces[i][1]][0]); //Bottom left
			glTexCoord2fv(text[i][2]);
			glVertex3fv(&v[faces[i][2]][0]); //Bottom right
			glTexCoord2fv(text[i][3]);
			glVertex3fv(&v[faces[i][3]][0]); //Top right
			glEnd();
		}
	}
	else {

		//Cara de davant
		glPushMatrix();
		glTranslatef(0, 0, midaZ - 1);
		for (i = 0; i < midaX; i++) {
			glPushMatrix();
			for (int j = 0; j < midaY; j++) {
				glBegin(GL_QUADS);
				glNormal3fv(&n[4][0]);
				glTexCoord2f(-0.5f, 0.5f);
				glVertex3fv(&v[faces[4][0]][0]); //Top left
				glTexCoord2f(-0.5f, -0.5f);
				glVertex3fv(&v[faces[4][1]][0]); //Bottom left
				glTexCoord2f(0.5f, -0.5f);
				glVertex3fv(&v[faces[4][2]][0]); //Bottom right
				glTexCoord2f(0.5f, 0.5f);
				glVertex3fv(&v[faces[4][3]][0]); //Top right
				glEnd();
				glTranslatef(0, 1.0f, 0);
			}
			glPopMatrix();
			glTranslatef(1.0f, 0, 0);
		}
		glPopMatrix();

		//Cara de darrera
		glPushMatrix();
		for (i = 0; i < midaX; i++) {
			glPushMatrix();
			for (int j = 0; j < midaY; j++) {
				glBegin(GL_QUADS);
				glNormal3fv(&n[5][0]);
				glTexCoord2f(-0.5f, 0.5f);
				glVertex3fv(&v[faces[5][0]][0]); //Top left
				glTexCoord2f(-0.5f, -0.5f);
				glVertex3fv(&v[faces[5][1]][0]); //Bottom left
				glTexCoord2f(0.5f, -0.5f);
				glVertex3fv(&v[faces[5][2]][0]); //Bottom right
				glTexCoord2f(0.5f, 0.5f);
				glVertex3fv(&v[faces[5][3]][0]); //Top right
				glEnd();
				glTranslatef(0, 1.0f, 0);
			}
			glPopMatrix();
			glTranslatef(1.0f, 0, 0);
		}
		glPopMatrix();

		//Cara esquerra
		glPushMatrix();
		for (i = 0; i < midaZ; i++) {
			glPushMatrix();
			for (int j = 0; j < midaY; j++) {
				glBegin(GL_QUADS);
				glNormal3fv(&n[0][0]);
				glTexCoord2f(-0.5f, 0.5f);
				glVertex3fv(&v[faces[0][0]][0]); //Top left
				glTexCoord2f(-0.5f, -0.5f);
				glVertex3fv(&v[faces[0][1]][0]); //Bottom left
				glTexCoord2f(0.5f, -0.5f);
				glVertex3fv(&v[faces[0][2]][0]); //Bottom right
				glTexCoord2f(0.5f, 0.5f);
				glVertex3fv(&v[faces[0][3]][0]); //Top right
				glEnd();
				glTranslatef(0, 1.0f, 0);
			}
			glPopMatrix();
			glTranslatef(0, 0, 1.0f);
		}
		glPopMatrix();
		
		//Cara dreta
		glPushMatrix();
		glTranslatef(midaX-1, 0, 0);
		for (i = 0; i < midaZ; i++) {
			glPushMatrix();
			for (int j = 0; j < midaY; j++) {
				glBegin(GL_QUADS);
				glNormal3fv(&n[2][0]);
				glTexCoord2f(-0.5f, 0.5f);
				glVertex3fv(&v[faces[2][0]][0]); //Top left
				glTexCoord2f(-0.5f, -0.5f);
				glVertex3fv(&v[faces[2][1]][0]); //Bottom left
				glTexCoord2f(0.5f, -0.5f);
				glVertex3fv(&v[faces[2][2]][0]); //Bottom right
				glTexCoord2f(0.5f, 0.5f);
				glVertex3fv(&v[faces[2][3]][0]); //Top right
				glEnd();
				glTranslatef(0, 1.0f, 0);
			}
			glPopMatrix();
			glTranslatef(0, 0, 1.0f);
		}
		glPopMatrix();

		//Cara de davall
		glPushMatrix();
		for (i = 0; i < midaX; i++) {
			glPushMatrix();
			for (int j = 0; j < midaZ; j++) {
				glBegin(GL_QUADS);
				glNormal3fv(&n[3][0]);
				glTexCoord2f(-0.5f, 0.5f);
				glVertex3fv(&v[faces[3][0]][0]); //Top left
				glTexCoord2f(-0.5f, -0.5f);
				glVertex3fv(&v[faces[3][1]][0]); //Bottom left
				glTexCoord2f(0.5f, -0.5f);
				glVertex3fv(&v[faces[3][2]][0]); //Bottom right
				glTexCoord2f(0.5f, 0.5f);
				glVertex3fv(&v[faces[3][3]][0]); //Top right
				glEnd();
				glTranslatef(0, 0, 1.0f);
			}
			glPopMatrix();
			glTranslatef(1.0f, 0, 0);
		}
		glPopMatrix();

		//Cara de damunt
		glPushMatrix();
		glTranslatef(0, midaY - 1, 0);
		for (i = 0; i < midaX; i++) {
			glPushMatrix();
			for (int j = 0; j < midaZ; j++) {
				glBegin(GL_QUADS);
				glNormal3fv(&n[1][0]);
				glTexCoord2f(-0.5f, 0.5f);
				glVertex3fv(&v[faces[1][0]][0]); //Top left
				glTexCoord2f(-0.5f, -0.5f);
				glVertex3fv(&v[faces[1][1]][0]); //Bottom left
				glTexCoord2f(0.5f, -0.5f);
				glVertex3fv(&v[faces[1][2]][0]); //Bottom right
				glTexCoord2f(0.5f, 0.5f);
				glVertex3fv(&v[faces[1][3]][0]); //Top right
				glEnd();
				glTranslatef(0, 0, 1.0f);
			}
			glPopMatrix();
			glTranslatef(1.0f, 0, 0);
		}
		glPopMatrix();
	}
}


//De graus a radians
float toRad(float degrees) {
    return (degrees * ((float)M_PI / 180.0f));
}

//De radians a graus
float toDegree(float rads) {
	return (rads * (180.0f / (float)M_PI));
}

void drawRoda() {
	glColor3f(0.5f, 0.5f, 0.5f);
	glLineWidth(8.0f);
	glBegin(GL_LINES);
	glVertex3f(0, -0.1f, 0);
	glVertex3f(0, 0.1f, 0);
	glVertex3f(0, 0, 0.1f);
	glVertex3f(0, 0, -0.1f);
	glVertex3f(0, 0.1f, 0.1f);
	glVertex3f(0, -0.1f, -0.1f);
	glVertex3f(0, 0.1f, -0.1f);
	glVertex3f(0, -0.1f, 0.1f);
	glEnd();

	glColor3f(0, 0, 0);
	glRotatef(90, 0, 1, 0);
	glutSolidTorus(0.1f, 0.2f, 10, 50);
}

//Dibuixa un cotxe
void drawCar(float rotRodes) {
	glFrontFace(GL_CCW);

	//Cos
	glColor3f(1, 0, 0);

	//Part de baix
	glNormal3f(0, -1, 0);
	//glPolygonMode(GL_FRONT, GL_QUADS);
	glBegin(GL_QUADS);
	glVertex3f(-1, 0, -3); //Top left
	glVertex3f(1, 0, -3); //Top right
	glVertex3f(1, 0, 1); //Bottom right
	glVertex3f(-1, 0, 1); //Bottom left
	glEnd();

	//Delantera
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
	glVertex3f(-1, 0.5f, 1); //Top left
	glVertex3f(-1, 0, 1); //Bottom left
	glVertex3f(1, 0, 1); //Bottom right
	glVertex3f(1, 0.5f, 1); //Top right
	glEnd();

	//Darrera
	glNormal3f(0, 0, -1);
	glBegin(GL_QUADS);
	glVertex3f(-1, 0.6f, -3); //Top left
	glVertex3f(1, 0.6f, -3); //Top right
	glVertex3f(1, 0, -3); //Bottom right
	glVertex3f(-1, 0, -3); //Bottom left
	glEnd();

	//Costat esquerra
	glNormal3f(-1, 0, 0);
	glBegin(GL_QUADS);
	glVertex3f(-1, 0.6f, 0); //Top left
	glVertex3f(-1, 0.6f, -3); //Top right
	glVertex3f(-1, 0, -3); //Bottom right
	glVertex3f(-1, 0, 0); //Bottom left
	glEnd();

	//Costat dret
	glNormal3f(1, 0, 0);
	glBegin(GL_QUADS);
	glVertex3f(1, 0.6f, 0); //Top left
	glVertex3f(1, 0, 0); //Bottom left
	glVertex3f(1, 0, -3); //Bottom right
	glVertex3f(1, 0.6f, -3); //Top right
	glEnd();

	//Cap�
	glNormal3f(0, 1, 0.1f);
	glBegin(GL_QUADS);
	glVertex3f(1, 0.5f, 1); //Top left
	glVertex3f(1, 0.6f, 0); //Bottom left
	glVertex3f(-1, 0.6f, 0); //Bottom right
	glVertex3f(-1, 0.5f, 1); //Top right
	glEnd();

	//Costat dret cap�
	glNormal3f(1, 0, 0);
	glBegin(GL_QUADS);
	glVertex3f(1, 0.5f, 1); //Top left
	glVertex3f(1, 0, 1); //Bottom left
	glVertex3f(1, 0, 0); //Bottom right
	glVertex3f(1, 0.6f, 0); //Top right
	glEnd();

	//Costat esquerra cap�
	glNormal3f(-1, 0, 0);
	glBegin(GL_QUADS);
	glVertex3f(-1, 0.5f, 1); //Top left
	glVertex3f(-1, 0.6f, 0); //Top right
	glVertex3f(-1, 0, 0); //Bottom right
	glVertex3f(-1, 0, 1); //Bottom left

	//Vidre
	glColor4f(0, 0, 1, 0.1f);
	glNormal3f(0, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(-1, 1, -0.5f); //Top left
	glVertex3f(-1, 0.6f, 0); //Bottom left
	glVertex3f(1, 0.6f, 0); //Bottom right
	glVertex3f(1, 1, -0.5f); //Top right
	glEnd();
	glColor3f(1, 0, 0);
	glLineWidth(5.0f);
	glNormal3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(-1, 1, -0.5f); //Top left
	glVertex3f(-1, 0.6f, 0); //Bottom left
	glVertex3f(1, 1, -0.5f); //Top right
	glVertex3f(1, 0.6f, 0); //Bottom right
	glEnd();

	//Part de dalt vidre
	glColor3f(1, 0, 0);
	glNormal3f(0, 1, 0);
	glBegin(GL_QUADS);
	glVertex3f(-1, 1, -1.5f); //Top left
	glVertex3f(-1, 1, -0.5f); //Bottom left
	glVertex3f(1, 1, -0.5f); //Bottom right
	glVertex3f(1, 1, -1.5f); //Top right
	glEnd();
	glColor3f(1, 0, 0);
	glLineWidth(5.0f);
	glNormal3f(-1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(-1, 1, -1); //Top left
	glVertex3f(-1, 0.6f, -1); //Bottom left
	glEnd();
	glNormal3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(1, 1, -1); //Top left
	glVertex3f(1, 0.6f, -1); //Bottom left
	glEnd();

	//Vidre costat esquerra
	glColor4f(0, 0, 1, 0.1f);
	glNormal3f(0, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(-1, 1, -1.5f); //Top left
	glVertex3f(-1, 0.6f, -2); //Bottom left
	glVertex3f(-1, 0.6f, 0); //Bottom right
	glVertex3f(-1, 1, -0.5f); //Top right
	glEnd();

	//Vidre costat dret
	glColor4f(0, 0, 1, 0.1f);
	glNormal3f(0, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(1, 1, -1.5f); //Top 
	glVertex3f(1, 1, -0.5f); //Top right
	glVertex3f(1, 0.6f, 0); //Bottom right
	glVertex3f(1, 0.6f, -2); //Bottom left
	glEnd();

	//vidre de darrera
	glColor4f(0, 0, 1, 0.1f);
	glNormal3f(0, 1, -1);
	glBegin(GL_QUADS);
	glVertex3f(-1, 0.6f, -2); //Top left
	glVertex3f(-1, 1, -1.5f); //Bottom left
	glVertex3f(1, 1, -1.5f); //Bottom right
	glVertex3f(1, 0.6f, -2); //Top right
	glEnd();
	glColor3f(1, 0, 0);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glVertex3f(-1, 0.6f, -2); //Top left
	glVertex3f(-1, 1, -1.5f); //Bottom left
	glVertex3f(1, 1, -1.5f); //Bottom right
	glVertex3f(1, 0.6f, -2); //Top right
	glEnd();

	//Part darrera vidre
	glColor3f(1, 0, 0);
	glNormal3f(0, 1, 0);
	glBegin(GL_QUADS);
	glVertex3f(-1, 0.6f, -3); //Top left
	glVertex3f(-1, 0.6f, -2); //Bottom left
	glVertex3f(1, 0.6f, -2); //Bottom right
	glVertex3f(1, 0.6f, -3); //Top right
	glEnd();
	glColor3f(1, 0, 0);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glVertex3f(-1, 0.6f, -3); //Top left
	glVertex3f(-1, 0.6f, -2); //Bottom left
	glVertex3f(1, 0.6f, -3); //Top right
	glVertex3f(1, 0.6f, -2); //Bottom right
	glEnd();

	//Rodes

	glPushMatrix();
	glTranslatef(1, -0.1f, -2.5f);
	glNormal3f(1, 0, 0);
	glRotatef(rotRodes, 0, 1, 0);
	drawRoda();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1, -0.1f, -2.5f);
	glNormal3f(-1, 0, 0);
	glRotatef(rotRodes, 0, 1, 0);
	drawRoda();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1, -0.1f, 0.5f);
	glNormal3f(1, 0, 0);
	glRotatef(rotRodes, 0, 1, 0);
	drawRoda();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1, -0.1f, 0.5f);
	glNormal3f(-1, 0, 0);
	glRotatef(rotRodes, 0, 1, 0);
	drawRoda();
	glPopMatrix();

	//Llum dret
	glColor3f(1, 1, 0);
	glPushMatrix();
	glTranslatef(0.8f, 0.3f, 1.1f);
	glRotatef(180, 0, 1, 0);
	glutSolidCone(0.1f, 0.2f, 4, 4);
	glPopMatrix();

	//Llum esquerra
	glColor3f(1, 1, 0);
	glPushMatrix();
	glTranslatef(-0.8f, 0.3f, 1.1f);
	glRotatef(180, 0, 1, 0);
	glutSolidCone(0.1f, 0.2f, 4, 4);
	glPopMatrix();

	//Tubo d'escape
	glColor3f(0.5f, 0.5f, 0.5f);
	glPushMatrix();
	glTranslatef(-0.8f, 0.2f, -3);
	glRotatef(180, 0, 1, 0);
	glutSolidCylinder(0.05f, 0.1f, 10, 10);
	glPopMatrix();
}