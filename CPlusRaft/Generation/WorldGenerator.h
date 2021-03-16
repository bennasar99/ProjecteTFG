#pragma once

#include "../Utils/Vector3.h"
#include "../lib/FastNoiseLite.h"

#define NBIOMES 9

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
	FastNoiseLite oceanNoise;

	FastNoiseLite normalNoise;
	FastNoiseLite oceanGenNoise;
	FastNoiseLite mountainNoise;

	FastNoiseLite caveNoise;

	World *world;
	
	int seed;

public:

	WorldGenerator();
	WorldGenerator(int seed, World* world);

	Bioma getBiomeAt(int cX, int cZ);

	float getDensity(Bioma bio, Vector3<int> pos);
	float getThreshold(Bioma bio);

	Chunk* generateDetail(Chunk* chunk);
	Chunk* generateTerrain(Vector3<int> cPos);

	static bool isBiome3D(Bioma bio);
};