#pragma once

#include "../Utils/Vector3.h"
#include "../lib/FastNoiseLite.h"
#include <string>
#include <random>

#define NBIOMES 10

enum class Bioma {
	MUNTANYA,
	BOSC,
	OCEA,
	MAR,
	PLANA,
	DESERT,
	GEL,
	ARTIC,
	BOSCNEVAT,
	SABANA
};

enum class Generator {
	DEFAULT,
	BIGBIOMES,
	OCEANWORLD
};

class World;
class Chunk;

class WorldGenerator {
private:
	FastNoiseLite climateNoise;
	FastNoiseLite biomeNoise;
	FastNoiseLite riverNoise;
	FastNoiseLite oceanNoise;
	FastNoiseLite seaNoise;

	FastNoiseLite normalNoise;
	FastNoiseLite oceanGenNoise;
	FastNoiseLite mountainNoise;
	FastNoiseLite caveNoise;
	FastNoiseLite oreNoise;

	World *world;
	Generator gen;
	
	float biomeSize = 1;
	float oceanProb = 0.1f;
	float seaToOcean = 0.1f;
	float climHotProb = 0.1f;
	float climWarmProb = 0.1f;
	float climColdProb = 0.1f;
	float caveSize = 0.1f;
	float caveProb = 0.1f;
	float oreProb = 0.1f;
	float oreSize = 0.1f;
	float riverProb = 0.1f;
	float riverSize = 0.1f;
	int seed;

public:

	WorldGenerator();
	WorldGenerator(int seed, World* world, Generator gen);

	Bioma getBiomeAt(int cX, int cZ);

	float getDensity(Bioma bio, Vector3<int> pos);
	float getThreshold(Bioma bio);

	Chunk* generateDetail(Chunk* chunk);
	Chunk* generateTerrain(Vector3<int> cPos);

	void setSeed(int seed);
	int getSeed();

	void readFromFile();

	static bool isBiome3D(Bioma bio);
};