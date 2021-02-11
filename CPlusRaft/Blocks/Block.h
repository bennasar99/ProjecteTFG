#pragma once

#include "../Render/ChunkMesh.h"
#include "../Utils/Vector3.h"


#define NBLOCS 25

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
	NEU,
	GEL,
	ARENA,
	OR,
	FANG,
	FERRO,
	GRAVILLA,
	CARBO,
	LIMIT //SEMPRE DARRER
};

class World;

class Block {
protected:

	Bloc id;
	//Block* parent = 0;	/*	Bloc pare: si un bloc t� un pare vol dir que el "fill" �s un bloc que nom�s serveix per ocupar 
							//espai (que no s'hi puguin col�locar blocs) i que en destruir-lo es destrueixi tota l'estructura */
public:

	virtual void update(float delta, World* world);
	virtual void destroy(World* world);
	virtual void interact(World* world);
	virtual void draw(ChunkMesh* cM, bool visible[6], Vector3<int> relPos);

	Bloc getId();
	void setId(Bloc id);

	static bool isTransparent(Bloc tipus);
	static bool isSolid(Bloc tipus);
	static bool canSeeThrough(Bloc tipus);

	Block(Bloc id);
	Block();
};