#include <GL/glew.h>
#include "Utils.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <cmath>
#include "Utils/TextureManager.h"

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

//Codi de: https://stackoverflow.com/questions/36949957/loading-a-wav-file-for-openal
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


//De graus a radians
float toRad(float degrees) {
    return (degrees * ((float)M_PI / 180.0f));
}

//De radians a graus
float toDegree(float rads) {
	return (rads * (180.0f / (float)M_PI));
}

void drawSphere(double r, int lats, int longs) {
	int i, j;
	for (i = 0; i <= lats; i++) {
		double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = M_PI * (-0.5 + (double)i / lats);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= longs; j++) {
			double lng = 2 * M_PI * (double)(j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3d(x * zr0, y * zr0, z0);
			glVertex3d(r * x * zr0, r * y * zr0, r * z0);
			glNormal3d(x * zr1, y * zr1, z1);
			glVertex3d(r * x * zr1, r * y * zr1, r * z1);
		}
		glEnd();
	}
}

//Agafat de https://stackoverflow.com/questions/22210684/16-bit-floats-and-gl-half-float
//Afegit cas per f == 0, sinó retorna 2
unsigned short toHFloat(float f)
{
	unsigned short   fltInt16 = 0;
	int     fltInt32;

	if (f == 0.0f) {
		return 0;
	}

	memcpy(&fltInt32, &f, sizeof(float));
	fltInt16 = ((fltInt32 & 0x7fffffff) >> 13) - (0x38000000 >> 13);
	fltInt16 |= ((fltInt32 & 0x80000000) >> 16);

	return fltInt16;
}

//EI!! Cas f == 0?
float toFloat(unsigned short fltInt16) {
	int fltInt32 = ((fltInt16 & 0x8000) << 16);
	fltInt32 |= ((fltInt16 & 0x7fff) << 13) + 0x38000000;

	float fRet;
	memcpy(&fRet, &fltInt32, sizeof(float));
	return fRet;
}

void drawString(std::string str, float size) {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::FONT));
	for (int i = 0; i < str.length(); i++) {
		char c = str.at(i);
		std::array<float, 4> tCoords;
		TextureManager::getFontTexCoords(c, tCoords);
		float xb = tCoords[0]; float yb = tCoords[1]; float xt = tCoords[2]; float yt = tCoords[3];
		//{{xt, yb}, {xb,yb}, {xb,yt}, {xt,yt}}
		glColor4f(1, 1, 1, 1);
		//glScalef(10, 10, 10);
		glBegin(GL_QUADS); //Quadrat blanc exterior
		glTexCoord2f(xb, yb);
		glVertex3f(0, size, -1);
		glTexCoord2f(xb, yt);
		glVertex3f(0, 0, -1);
		glTexCoord2f(xt, yt);
		glVertex3f(size, 0, -1);
		glTexCoord2f(xt, yb);
		glVertex3f(size, size, -1);
		glEnd();
		if (c == 'i' || c == 'j' || c == 'l' || c == 'I' || c == 't' || c == 'f') {
			glTranslatef(size / 5.0f, 0, 0);
		}
		else if (c == 'T' || c == 'E' || c == 'F' || c == 'S' || c == 'L' || islower(c)) {
			glTranslatef(size / 2.5f, 0, 0);
		}
		else {
			glTranslatef(size / 2.0f, 0, 0);
		}
		//glScalef(0.1f, 0.1f, 0.1f);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}