#pragma once

#include "../Vector3.h"
#include "../lib/FastNoiseLite.h"


enum class Bioma {
	MUNTANYA,
	NEUTRAL,
	OCEA,
	PLANA
};

class World;
class Chunk;

class WorldGenerator {
private:
	FastNoiseLite biomeNoise;
	FastNoiseLite heightNoise;

	World *world;
	
public:

	WorldGenerator();
	WorldGenerator(int seed, World* world);

	Bioma getBiomeAt(int cX, int cZ);
	
	bool generateDetail(Chunk* chunk);
	Chunk* generateTerrain(Vector3<int> cPos);
};