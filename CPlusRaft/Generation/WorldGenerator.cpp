#include "WorldGenerator.h"
#include "../world.h"
#include "../Chunk.h"

WorldGenerator::WorldGenerator(int seed, World* world) {
	this->seed = seed;

	srand(seed);

	//Oceans
	this->oceanNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->oceanNoise.SetSeed(seed*4);
	this->oceanNoise.SetFrequency(0.04f); //0.04

	//Clima (calor, templat, fred)
	this->climateNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	this->climateNoise.SetSeed(seed*3);
	this->climateNoise.SetFrequency(0.1f); //0.01
	this->climateNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);

	//Bioma (depen del clima)
	this->biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	this->biomeNoise.SetSeed(seed*2);
	this->biomeNoise.SetFrequency(0.1f); //0.1
	this->biomeNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);

	this->normalNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->oceanGenNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->mountainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->normalNoise.SetSeed(seed);
	this->mountainNoise.SetSeed(seed);
	this->oceanGenNoise.SetSeed(seed);
	this->normalNoise.SetFrequency(0.01f); //0.01
	this->mountainNoise.SetFrequency(0.02f);
	this->oceanGenNoise.SetFrequency(0.002f);

	this->world = world;
}

WorldGenerator::WorldGenerator() {
	
}

Bioma WorldGenerator::getBiomeAt(int cX, int cZ) {
	float ocean = oceanNoise.GetNoise((float)cX, (float)cZ);
	float climate = climateNoise.GetNoise((float)cX, (float)cZ);
	float biome = biomeNoise.GetNoise((float)cX, (float)cZ);
	/*GLfloat ocean[16*16];
	GLfloat climate[16*16];
	GLfloat biome[16*16];

	biomeGen->GenUniformGrid2D(biome, cX, cZ, 16, 16, 0.1f, this->seed);
	climateGen->GenUniformGrid2D(climate, cX, cZ, 16, 16, 0.1f, this->seed);
	oceanGen->GenUniformGrid2D(ocean, cX, cZ, 16, 16, 0.04f, this->seed);*/

	if (ocean < -0.1f) {
		return Bioma::OCEA;
	}
	else if (ocean < 0) {
		return Bioma::MAR;
	}

	if (climate < -0.3f) { //Clima fred
		return Bioma::ARTIC;
	}
	else if (climate >= -0.3f && climate < 0.25f) { //Clima templat
		if (biome < -0.25f) {
			return Bioma::PLANA;
		}
		else if (biome >= -0.25f && biome < 0.25f) {
			return Bioma::BOSC;
		}
		else /*if (biome >= 0.5f)*/ {
			return Bioma::MUNTANYA;
		}
	}
	else if (climate >= 0.25f) { //Clima calent
		if (biome < -0.25f) {
			return Bioma::DESERT;
		}
		else if (biome >= -0.25f && biome < 0.3f) {
			return Bioma::SABANA;
		}
		else /*if (biome >= 0.5f)*/ {
			return Bioma::MUNTANYA;
		}
	}
}

Chunk* WorldGenerator::generateDetail(Chunk* chunk) { //Estructures, els chunks dels voltants ja estan generats
	Vector3<int> cPos = chunk->getPos();
	float density = 0;
	int randmax; //Com més alt, - densitat de coses
	Bioma bio = chunk->getBiome();
	switch (bio) {
	case Bioma::BOSC:
		randmax = 20;
		break;
	case Bioma::PLANA:
		randmax = 100;
		break;

	}
	Vector3<int> pos = Vector3<int>(0, 0, 0);
	for (pos.x = 0; pos.x < CHUNKSIZE; pos.x++) {
		for (pos.z = 0; pos.z < CHUNKSIZE; pos.z++) {
			//Intentar que com més abaix + (molt més) probable que sigui sòlid
			//density = heightNoise->GetNoise((float)pos.x + CHUNKSIZE * (float)cPos.x, CHUNKSIZE * (float)cPos.y, (float)pos.z + CHUNKSIZE * (float)cPos.z);
			//if (density > threshold) { //Afegim cosa
				//Agafam la coordenada Y amb un bloc i aire damunt més gran
				bool trobat = false;
				for (pos.y = 15; (pos.y >= 0)/*&&(!trobat)*/; pos.y--) {
					Vector3<int> tpos = cPos * CHUNKSIZE + pos + Vector3<int>(0, 1, 0);
					Bloc b1 = chunk->getBlock(pos);
					Bloc b2 = chunk->getBlockWorld(tpos);
					if (b1 == Bloc::TERRA && b2 == Bloc::RES){
						trobat = true;
						int random = rand() % randmax;
						if (random == 4 || random == 5 || random == 6 || random == 7 || random == 8) {
							//chunk->setBlock(new SpreadBlock(Bloc::HERBA, tpos), tpos);
							world->setBlock(Bloc::HERBA, tpos, nullptr, false);
						}
						else if (random == 9) {
							//Tronc
							//chunk->setBlock(new SolidBlock(Bloc::FUSTAARBRE), tpos);
							world->setBlock(Bloc::FUSTAARBRE, tpos, nullptr, false);
							int rand2 = rand() % 5 + 1;
							for (int i = 1; i <= rand2; i++) {
								Vector3<int> fpos = tpos + Vector3<int>(0, i, 0);
								world->setBlock(Bloc::FUSTAARBRE, fpos, nullptr, false);
								/*if (fpos.y >= 0 && fpos.y < 15) {
									chunk->setBlock(new SolidBlock(Bloc::FUSTAARBRE), fpos);
								}*/
							}
							//Fulles
							int altura = rand() % 3 + 1;
							int amplada = (rand() % (rand2)) + 1;
							for (int y = 1; y <= altura; y++) {
								for (int x = -amplada; x <= amplada; x++) {
									for (int z = -amplada; z <= amplada; z++) {
										Vector3<int> lpos = tpos + Vector3<int>(0, rand2 + y, 0)
											+ Vector3<int>(x, 0, 0) + Vector3<int>(0, 0, z);
										if (chunk->getBlockWorld(lpos) == Bloc::RES) {
											world->setBlock(Bloc::FULLAARBRE, lpos, nullptr, false);
										}
									}
								}
							}
						}
					}
				}
			//}
		}
	}
	chunk->updateMesh();
	return chunk;
}

Chunk* WorldGenerator::generateTerrain(Vector3<int> cPos){ //Sense estructures, només terreny

	Chunk* chunk = new Chunk(this->world, cPos);

	chunk->setBiome(getBiomeAt(cPos.x, cPos.z));
	Bioma bio = chunk->getBiome();
	
	FastNoiseLite* noise = &this->normalNoise; //Deixar a null

	int sealvl = 80;
	float y = 0;
	float density = 0;
	float threshold = 1;
	switch (bio) {
	case Bioma::MUNTANYA:
		noise = &this->mountainNoise;
		threshold = 1.4f;
		break;
	case Bioma::OCEA:
		noise = &this->oceanGenNoise;
		threshold = 0.5f;
		break;
	default:
		noise = &this->normalNoise;
		break;
	}
	

	bool block = false;
	int waterblocksup = 0;
	int nblocs = 0;
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int y = 0; y < CHUNKSIZE; y++) {
				Vector3<int> pos = Vector3<int>(x, y, z);
				//Intentar que com més abaix + (molt més) probable que sigui sòlid
				density = (float(CHUNKSIZE*cPos.y + y)/80.0f) + noise->GetNoise((float)x + CHUNKSIZE*(float)cPos.x, (float)y + CHUNKSIZE * (float)cPos.y, float(z) + CHUNKSIZE * (float)cPos.z)/1.5f;
				if (density < threshold) {
					if (CHUNKSIZE * cPos.y + y > sealvl) {
						if (bio == Bioma::MAR || bio == Bioma::OCEA) {
							continue;
						}
					}
					nblocs++;
					if (bio == Bioma::MUNTANYA) {
						chunk->setBlock(new SolidBlock(Bloc::PEDRA), pos);
					}
					else if (bio == Bioma::ARTIC) {
						chunk->setBlock(new SolidBlock(Bloc::NEU), pos);
					}
					else {
						chunk->setBlock(new SolidBlock(Bloc::TERRA), pos);
					}
				}
				else {
					if ((CHUNKSIZE*cPos.y + y) <= sealvl) {
						if ((CHUNKSIZE * cPos.y + y) == sealvl && bio == Bioma::ARTIC) {
							chunk->setBlock(new SolidBlock(Bloc::GEL), pos);
						}
						else {
							chunk->setBlock(new LiquidBlock(Bloc::AIGUA, pos), pos);
						}
						nblocs++;
					}
					if ((CHUNKSIZE * cPos.y + y) == sealvl) {
						waterblocksup++;
					}
				}
			}
		}
	}
	if (waterblocksup > (16 * 16) / 2 && chunk->getBiome() != Bioma::OCEA) {
		if (bio != Bioma::ARTIC) {
			chunk->setBiome(Bioma::MAR);
		}
		else {
			chunk->setBiome(Bioma::GEL);
		}
	}
	if (nblocs == 0) {
		//return nullptr;
	}
	return chunk;
}