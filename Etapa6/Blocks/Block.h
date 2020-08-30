#pragma once
#include "../TextureManager.h"
#include "../ModelManager.h"

enum class Bloc {
	AIRE,
	AIGUA,
	CUB,
	CONO,
	ESFERA,
	TASSA,
	CONO4C,
	ESFERA4C,
	DONUT,
	HERBA,
	HERBAFULL,
	TERRA,
	VIDRE,
	TORXA,
	LLUMSOTIL,
	LLUMTERRA,
	PENDUL,
	FAROLA,
	MULTICOLOR,
	FULLAARBRE,
	FUSTA,
	FUSTAARBRE,
	PEDRA,
	NORIA,
	ALTAVEU,
	ESTALAGMITA,
	MIRALL,
	GRUA,
	RES //Sempre ha d'anar el darrer
};

class World;

class Block {
protected:

	Bloc id;
	World *world;
	Block* parent = 0;	/*	Bloc pare: si un bloc té un pare vol dir que el "fill" és un bloc que només serveix per ocupar 
							espai (que no s'hi puguin col·locar blocs) i que en destruir-lo es destrueixi tota l'estructura */
public:

	virtual void draw(bool wireframe);
	virtual void draw();
	virtual void update(int delta);
	virtual void destroy();
	virtual void interact();

	Block* getParent();

	Bloc getId();
	void setId(Bloc id);

	static bool isTransparent(Bloc tipus);

	Block(World *world, Bloc id, Block* parent);
	Block();
};