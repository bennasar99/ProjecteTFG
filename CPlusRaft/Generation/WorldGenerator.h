#pragma once

#include "../Utils/Vector3.h"
#include "../lib/FastNoiseLite.h"
#include <string>

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

	World *world;
	Generator gen;
	
	float biomeSize = 1;
	float oceanProb;
	float seaToOcean;
	float climHotProb;
	float climWarmProb;
	float climColdProb;
	float caveSize;
	float caveProb;
	float oreProb;

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