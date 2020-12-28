#include "WorldGenerator.h"
#include "../world.h"
#include "../Chunk.h"

WorldGenerator::WorldGenerator(int seed, World* world) {
	srand(seed);

	//Oceans
	this->oceanNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->oceanNoise.SetSeed(seed*4);
	this->oceanNoise.SetFrequency(0.05f);

	//Clima (calor, templat, fred)
	this->climateNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->climateNoise.SetSeed(seed*3);
	this->climateNoise.SetFrequency(0.01f);

	//Bioma (depen del clima)
	this->biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->biomeNoise.SetSeed(seed*2);
	this->biomeNoise.SetFrequency(0.1f);

	this->heightNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->heightNoise.SetSeed(seed);
	this->heightNoise.SetFrequency(0.01f);

	this->world = world;
}

WorldGenerator::WorldGenerator() {
	
}

Bioma WorldGenerator::getBiomeAt(int cX, int cZ) {
	float ocean = oceanNoise.GetNoise((float)cX, (float)cZ);
	float climate = climateNoise.GetNoise((float)cX, (float)cZ);
	float biome = biomeNoise.GetNoise((float)cX, (float)cZ);

	if (ocean < 0.05f) {
		return Bioma::OCEA;
	}

	if (climate < -0.25f) { //Clima fred
		if (biome < 0) {
			return Bioma::GEL;
		}
		else {
			return Bioma::MUNTGEL;
		}
	}
	else if (climate >= -0.25f && climate < 0.25f) { //Clima templat
		if (biome < -0.3f) {
			return Bioma::OCEA;
		}
		else if (biome >= -0.3f && biome < 0.1f) {
			return Bioma::PLANA;
		}
		else if (biome >= 0.1f && biome < 0.5f) {
			return Bioma::BOSC;
		}
		else /*if (biome >= 0.5f)*/ {
			return Bioma::MUNTANYA;
		}
	}
	else if (climate >= 0.25f) { //Clima calent
		if (biome < -0.3f) {
			return Bioma::OCEA;
		}
		else if (biome >= -0.3f && biome < 0.1f) {
			return Bioma::DESERT;
		}
		else if (biome >= 0.1f && biome < 0.6f) {
			return Bioma::SABANA;
		}
		else /*if (biome >= 0.5f)*/ {
			return Bioma::MUNTANYA;
		}
	}
}

bool WorldGenerator::generateDetail(Chunk* chunk) { //Estructures, els chunks dels voltants ja estan generats
	Vector3<int> cPos = chunk->getPos();
	float density = 0;
	float threshold = 0.3f;
	Vector3<int> pos = Vector3<int>(0, 0, 0);
	for (pos.x = 0; pos.x < CHUNKSIZE; pos.x++) {
		for (pos.z = 0; pos.z < CHUNKSIZE; pos.z++) {
			//Intentar que com més abaix + (molt més) probable que sigui sòlid
			density = heightNoise.GetNoise((float)pos.x + CHUNKSIZE * (float)cPos.x, CHUNKSIZE * (float)cPos.y, (float)pos.z + CHUNKSIZE * (float)cPos.z);
			if (density < threshold) { //Afegim cosa
				//Agafam la coordenada Y amb un bloc i aire damunt més gran
				bool trobat = false;
				for (pos.y = 14; (pos.y >= 0)/*&&(!trobat)*/; pos.y--) {
					Vector3<int> tpos = pos + Vector3<int>(0, 1, 0);
					Bloc b1 = chunk->getBlock(pos);
					Bloc b2 = chunk->getBlock(tpos);
					if (b1 == Bloc::TERRA && b2 == Bloc::RES){
						trobat = true;
						int random = rand() % 128;
						if (random == 4 || random == 5 || random == 6 || random == 7 || random == 8) {
							chunk->setBlock(new SpreadBlock(Bloc::HERBA, tpos), tpos);
						}
						else if (random == 9) {
							//Tronc
							chunk->setBlock(new SolidBlock(Bloc::FUSTAARBRE), tpos);
							int rand2 = rand() % 5 + 1;
							for (int i = 1; i <= rand2; i++) {
								Vector3<int> fpos = tpos + Vector3<int>(0, i, 0);
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
										Vector3<int> lpos = tpos + Vector3<int>(0, rand2 + y, 0)
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
			}
		}
	}
	return true;
}

Chunk* WorldGenerator::generateTerrain(Vector3<int> cPos){ //Sense estructures, només terreny
	Chunk* chunk = new Chunk(this->world, cPos);
	//chunk->setBiome(getBiomeAt(cX, cZ));
	chunk->setBiome(getBiomeAt(cPos.x, cPos.z));

	float y = 0;
	float density = 0;
	float threshold = 0.99f;
	switch (chunk->getBiome()) {
	case Bioma::MUNTANYA:
		this->heightNoise.SetFrequency(0.02f);
		break;
	default:
		this->heightNoise.SetFrequency(0.005f);
		break;
	}
	//Vector3<int> cPos = chunk->getPos();
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int y = 0; y < CHUNKSIZE; y++) {
				Vector3<int> pos = Vector3<int>(x, y, z);
				//Intentar que com més abaix + (molt més) probable que sigui sòlid
				density = (float(CHUNKSIZE*cPos.y + y)/80.0f) + heightNoise.GetNoise((float)x + CHUNKSIZE*(float)cPos.x, (float)y + CHUNKSIZE * (float)cPos.y, float(z) + CHUNKSIZE * (float)cPos.z)/1.5f;
				if (density < threshold) {
					//printf("TERRA\n");
					if (chunk->getBiome() == Bioma::MUNTANYA) {
						chunk->setBlock(new SolidBlock(Bloc::PEDRA), pos);
					}
					else {
						chunk->setBlock(new SolidBlock(Bloc::TERRA), pos);
					}
				}
				else {
					if ((CHUNKSIZE*cPos.y + y) < 80) {
						chunk->setBlock(new LiquidBlock(Bloc::AIGUA, pos), pos);
					}
				}
			}
		}
	}
	return chunk;
}