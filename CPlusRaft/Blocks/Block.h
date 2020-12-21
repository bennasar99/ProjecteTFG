#pragma once

#include "../Render/ChunkMesh.h"
#include "../Vector3.h"

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
	FULLAARBRE,
	FUSTA,
	FUSTAARBRE,
	PEDRA,
	ALTAVEU,
	LIMIT //SEMPRE DARRER
};

class World;

class Block {
protected:

	Vector3<float> pos;
	Bloc id;
	World *world;
	Block* parent = 0;	/*	Bloc pare: si un bloc té un pare vol dir que el "fill" és un bloc que només serveix per ocupar 
							espai (que no s'hi puguin col·locar blocs) i que en destruir-lo es destrueixi tota l'estructura */
public:

	virtual void update(int delta);
	virtual void destroy();
	virtual void interact();
	virtual void draw(ChunkMesh* cM, bool visible[6], Vector3<float> relPos);

	Block* getParent();

	Bloc getId();
	void setId(Bloc id);

	static bool isTransparent(Bloc tipus);
	static bool isSolid(Bloc tipus);

	Block(World *world, Bloc id, Block* parent);
	Block();
};