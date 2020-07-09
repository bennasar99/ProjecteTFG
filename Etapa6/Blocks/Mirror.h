#pragma once
#include "../Vector3.h"
#include "Block.h"
#include <list>
#include "../Camera.h"

class Block;
class World;
class Camera;

/*
	Mirall
*/
class Mirror : public Block{
private:

	Vector3 pos; //Posició del mirall

	GLuint fbo, rbo, tfbo; //Framebuffer, Renderbuffer i textura generada
	
	Camera cam = Camera(Vector3(0, 0, 0), Vector3(0, 0, 0));
public:
	Mirror(World *world, Vector3 pos);

	void update(int delta);

	void draw(bool wireframe);
	void destroy();
};

