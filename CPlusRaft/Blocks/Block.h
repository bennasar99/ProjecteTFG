#pragma once

#include "../Render/ChunkMesh.h"
#include "../Utils/Vector3.h"
#include "../Utils/ShaderManager.h"
#include "../Render/MarchingCubes.h"
#include <array>
#include "../Render/Shader.h"

class Chunk;

#define NBLOCS 26

enum class Bloc :uint8_t {
	RES,
	AIGUA,
	CUB,
	HERBA,
	TERRA,
	VIDRE,
	TORXA,
	LLUMSOTIL,
	LLUMTERRA,
	FULLAARBRE,
	FUSTA,
	FUSTAARBRE,
	PEDRAW, //Walk Stone
	PEDRAS, //Smooth Stone
	PEDRA, //Stone
	ALTAVEU,
	NEU,
	NEUSUP,
	GEL,
	ARENA,
	OR,
	FANG,
	FERRO,
	GRAVILLA,
	CARBO,
	LIMIT //SEMPRE DARRER
};

enum class Cara {
	ESQUERRA,
	ADALT,
	DRETA,
	ABAIX,
	DAVANT,
	DARRERA
};

class World;

class Block {
protected:
	static bool marching;

	Bloc id;
	//Block* parent = 0;	/*	Bloc pare: si un bloc té un pare vol dir que el "fill" és un bloc que només serveix per ocupar 
							//espai (que no s'hi puguin col·locar blocs) i que en destruir-lo es destrueixi tota l'estructura */
public:

	virtual void update(float delta, World* world);
	virtual void destroy(World* world);
	virtual void interact(World* world);
	virtual void draw(ChunkMesh* cM, bool visible[6], Vector3<int> relPos) {};

	Bloc getId();
	void setId(Bloc id);

	static void draw(Bloc id, ChunkMesh* cM, Vector3<int> relPos, bool visible[6]);
	static bool getBlockInfo(Bloc id, std::array<float, 4>& texCoords, std::array<unsigned char, 4>& color);
	static bool isTransparent(Bloc tipus);
	static bool isSolid(Bloc tipus);
	static bool isInteractable(Bloc tipus);
	static bool isCube(Bloc tipus);
	static bool canSeeThrough(Bloc tipus);
	static bool isMarcheable(Bloc tipus);
	static bool drawIcon(Bloc tipus);

	static void drawBlock(Bloc id, ChunkMesh* cM, Vector3<int> relPos, bool visible[6]);

	//Marching cubes
	static void drawMarching(Bloc id, ChunkMesh* cM, Vector3<int> relPos, Chunk* cnk);
	static void setMCEnabled(bool set);
	static bool getMCEnabled();

	Block(Bloc id);
	Block();
};