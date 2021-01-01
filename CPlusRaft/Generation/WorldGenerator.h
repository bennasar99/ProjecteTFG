#pragma once

#include "../Vector3.h"
#include "../lib/FastNoiseLite.h"


enum class Bioma {
	MUNTANYA,
	BOSC,
	OCEA,
	MAR,
	PLANA,
	SABANA,
	DESERT,
	GEL,
	ARTIC
};

class World;
class Chunk;

class WorldGenerator {
private:
	FastNoiseLite climateNoise;
	FastNoiseLite biomeNoise;
	FastNoiseLite heightNoise;
	FastNoiseLite oceanNoise;

	World *world;
	
public:

	WorldGenerator();
	WorldGenerator(int seed, World* world);

	Bioma getBiomeAt(int cX, int cZ);
	
	bool generateDetail(Chunk* chunk);
	Chunk* generateTerrain(Vector3<int> cPos);
};