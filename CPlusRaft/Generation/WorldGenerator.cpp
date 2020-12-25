#include "WorldGenerator.h"
#include "../world.h"
#include "../Chunk.h"

WorldGenerator::WorldGenerator(int seed, World* world) {
	srand(seed);
	this->biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->biomeNoise.SetSeed(seed);
	this->heightNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->heightNoise.SetSeed(seed);
	this->heightNoise.SetFrequency(0.01f);

	this->world = world;
}

WorldGenerator::WorldGenerator() {
	
}

Bioma WorldGenerator::getBiomeAt(int cX, int cZ) {
	float biome = biomeNoise.GetNoise((float)cX, (float)cZ);
	if (biome < -0.5f) {
		return Bioma::MUNTANYA;
	}
	else if (biome >= -0.5f && biome < 0) {
		return Bioma::NEUTRAL;
	}
	else if (biome >= 0 && biome < 0.5f) {
		return Bioma::PLANA;
	}
	else /*if (biome >= 0.5f)*/ {
		return Bioma::OCEA;
	}
}

Chunk* WorldGenerator::generate(int cX, int cY, int cZ){
	Chunk* chunk = new Chunk(this->world, Vector3<int>(cX, cY, cZ));
	chunk->setBiome(getBiomeAt(cX, cZ));
	float y = 0;
	float density = 0;
	float threshold = 0.99f;
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int z = 0; z < CHUNKSIZE; z++) {
			float lastdensity = 1;
			for (int y = 0; y < CHUNKSIZE; y++) {
				Vector3<int> pos = Vector3<int>(x, y, z);
				//Intentar que com més abaix + (molt més) probable que sigui sòlid
				density = (float(CHUNKSIZE*cY + y)/80.0f) + heightNoise.GetNoise((float)x + CHUNKSIZE*(float)cX, (float)y + CHUNKSIZE * (float)cY, float(z) + CHUNKSIZE * (float)cZ)/1.5f;
				if (density < threshold) {
					//printf("TERRA\n");
					chunk->setBlock(new SolidBlock(Bloc::TERRA), pos);
				}
				else {
					if ((CHUNKSIZE*cY + y) < 80) {
						chunk->setBlock(new LiquidBlock(Bloc::AIGUA, pos), pos);
					}
					else if (lastdensity < threshold) {
						int random = rand() % 128;
						if (random == 4 || random == 5) {
							chunk->setBlock(new SpreadBlock(Bloc::HERBA, pos), pos);
						}

						if (random == 6) {
							//Tronc
							chunk->setBlock(new SolidBlock(Bloc::FUSTAARBRE), pos);
							int rand2 = rand() % 5 + 1;
							for (int i = 1; i <= rand2; i++) {
								Vector3<int> fpos = pos + Vector3<int>(0, i, 0);
								if (fpos.y >= 0 && fpos.y < 15) {
									chunk->setBlock(new SolidBlock(Bloc::FUSTAARBRE), fpos);
								}
							}
							//Fulles
							int altura = rand() % 3 + 1;
							int amplada = (rand() % (rand2)) + 1;
							for (int y = 1; y <= altura; y++) {
								for (int x = -amplada; x <= amplada; x++) {
									for (int z = -amplada; z <= amplada; z++) {
										Vector3<int> lpos = pos + Vector3<int>(0, rand2 + y, 0)
											+ Vector3<int>(x, 0, 0) + Vector3<int>(0, 0, z);
										if (lpos.z >= 0 && lpos.z < 16 && lpos.x >= 0 && lpos.x < 16 && lpos.y >= 0 && lpos.y < 16) {
											chunk->setBlock(new SolidBlock(Bloc::FULLAARBRE), lpos);
										}
									}
								}
							}
						}
					}
				}
				lastdensity = density;
			}
		}
	}
	return chunk;
}