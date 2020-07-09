#pragma once
#include "../Vector3.h"
#include "Block.h"
#include "../Light.h"
#include "../SoundManager.h"

class World;

/*
	Bloc amb NURBS
*/
class SurfaceBlock : public Block {
private:

	GLfloat puntssup[4][4][3] = { //Punts que formen la superfície NURBS
	{{-0.4f, -0.5f, -0.4f}, {-0.3f, -0.5f,-0.5f }, {0.3f, -0.5f, -0.5f }, {0.4f, -0.5f,-0.4f}},
	{{-0.5f, -0.5f, -0.5f}, {-0.3f, 2.5f,-0.3f }, {0.3f, 2.5f, -0.3f }, {0.5f, -0.5f,-0.5f}},
	{{-0.5f, -0.5f,  0.5f}, {-0.3f, 2.5f, 0.3f }, {0.3f, 2.5f,  0.3f }, {0.5f, -0.5f, 0.5f}},
	{{-0.4f, -0.5f,  0.4f}, {-0.3f, -0.5f, 0.5f }, {0.3f, -0.5f,  0.5f }, {0.4f, -0.5f, 0.4f}}
	};

	GLUnurbs* sup = NULL; //Superfície NURBS

public:
	SurfaceBlock(World* world, Bloc id);

	void destroy();

	void draw();
};

#pragma once
