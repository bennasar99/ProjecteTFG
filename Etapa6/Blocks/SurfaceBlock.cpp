#include "SurfaceBlock.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

SurfaceBlock::SurfaceBlock(World* world, Bloc id) : Block(world, id, 0) {
    switch (id) {
    case Bloc::ESTALAGMITA:
        sup = gluNewNurbsRenderer();
		gluNurbsProperty(sup, GLU_SAMPLING_TOLERANCE, 25.0);
		gluNurbsProperty(sup, GLU_DISPLAY_MODE, GLU_FILL);
		gluNurbsProperty(sup, GLU_CULLING, GL_TRUE);
        break;
    }
}

void SurfaceBlock::destroy() { 
    gluDeleteNurbsRenderer(sup); //Eliminam la superfície NURBS
    sup = NULL;
}

//Funció de dibuixat
void SurfaceBlock::draw() {
	glEnable(GL_AUTO_NORMAL);
	glMapGrid2f(10, 0.0, 1.0, 10, 0.0, 1.0);
	GLfloat knots[8] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 };
	glColor3f(0.5f, 0.35f, 0.05f);
	glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::ESTALAGMITA));
	gluBeginSurface(sup); //Dibuixam la superfície
	gluNurbsSurface(sup,
		8, knots, 8, knots,
		4 * 3, 3, &puntssup[0][0][0],
		4, 4, GL_MAP2_TEXTURE_COORD_3);
	gluNurbsSurface(sup,
		8, knots, 8, knots,
		4 * 3, 3, &puntssup[0][0][0],
		4, 4, GL_MAP2_VERTEX_3);
	gluEndSurface(sup);
	glDisable(GL_AUTO_NORMAL);
}

