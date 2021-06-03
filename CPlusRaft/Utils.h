#pragma once

#include "Utils/Vector3.h"
#include <string>
#include "lib/stb_image.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <math.h>
#include "Utils/TextureManager.h"

# define M_PI           3.14159265358979323846

void draw3dRect(float midaX, float midaY, float midaZ);
void drawCub(bool cares[6], int texNum);
void drawCub(bool cares[6]);

char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size);

void drawCar(float rotRodes);

float toRad(float degrees);
float toDegree(float rads);

void drawSphere(double r, int lats, int longs);
unsigned short toHFloat(float f);
float toFloat(unsigned short h);

void drawString(std::string str, float size);