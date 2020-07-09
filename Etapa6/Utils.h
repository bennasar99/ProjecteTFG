#pragma once

#include "Vector3.h"
#include <string>
#include "lib/stb_image.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <math.h>

# define M_PI           3.14159265358979323846

void draw3dRect(float midaX, float midaY, float midaZ);

char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size);

void drawCar(float rotRodes);

float toRad(float degrees);
float toDegree(float rads);