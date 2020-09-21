#pragma once
#include "../TextureManager.h"
#include "../ModelManager.h"

enum class Bloc {
	RES,
	AIRE,
	AIGUA,
	CUB,
	HERBA,
	HERBAFULL,
	TERRA,
	VIDRE,
	TORXA,
	LLUMSOTIL,
	LLUMTERRA,
	MULTICOLOR,
	FULLAARBRE,
	FUSTA,
	FUSTAARBRE,
	PEDRA,
	NORIA,
	ALTAVEU,
	MIRALL,
	GRUA,
};

class World;

class Block {
protected:

	Bloc id;
	World *world;
	Block* parent = 0;	/*	Bloc pare: si un bloc té un pare vol dir que el "fill" és un bloc que només serveix per ocupar 
							espai (que no s'hi puguin col·locar blocs) i que en destruir-lo es destrueixi tota l'estructura */
public:

	virtual void draw(bool visible[6]);
	virtual void draw();
	virtual void update(int delta);
	virtual void destroy();
	virtual void interact();

	Block* getParent();

	Bloc getId();
	void setId(Bloc id);

	static bool isTransparent(Bloc tipus);
	static bool isSolid(Bloc tipus);

	Block(World *world, Bloc id, Block* parent);
	Block();
};