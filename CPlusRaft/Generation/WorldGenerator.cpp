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
	this->climateNoise.SetFrequency(0.05f); //0.01
	this->climateNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);

	//Bioma (depen del clima)
	this->biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	this->biomeNoise.SetSeed(seed*2);
	this->biomeNoise.SetFrequency(0.05f); //0.1
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

float WorldGenerator::getDensity(Bioma bio, Vector3<int> pos) {
	FastNoiseLite* noise = &this->normalNoise; //Deixar a null
	float sealvl = 80;
	switch(bio){
	case Bioma::MUNTANYA:
		sealvl = 120;
		noise = &this->mountainNoise;
		break;
	case Bioma::OCEA:
		sealvl = 50;
		noise = &this->oceanGenNoise;
		break;
	default:
		noise = &this->normalNoise;
		break;
	}
	return (pos.y / sealvl + noise->GetNoise((float)pos.x, (float)pos.y, (float)pos.z));

}

float WorldGenerator::getThreshold(Bioma bio) {
	float threshold = 0;
	switch (bio) {
	case Bioma::MUNTANYA:
		threshold = 1.4f;
		break;
	case Bioma::OCEA:
		threshold = 0.5f;
		break;
	default:
		threshold = 1.0f;
		break;
	}
	return threshold;
}

Chunk* WorldGenerator::generateTerrain(Vector3<int> cPos){ //Sense estructures, només terreny

	Chunk* chunk = new Chunk(this->world, cPos);

	chunk->setBiome(getBiomeAt(cPos.x, cPos.z));
	Bioma bio = chunk->getBiome();

	bool transition = false;
	for (int nX = cPos.x - 1; nX <= cPos.x + 1; nX++) {
		for (int nZ = cPos.z - 1; nZ <= cPos.z + 1; nZ++) {
			if (getBiomeAt(nX, nZ) != bio) {
				transition = true;
			}
		}
	}
	
	FastNoiseLite* noise = &this->normalNoise; //Deixar a null

	int sealvl = 80;
	
	//Bioma bioN[8];

	bool block = false;
	int waterblocksup = 0;
	int nblocs = 0;
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int y = 0; y < CHUNKSIZE; y++) {

				//float threshold = getThreshold(bio);

				Vector3<int> pos = Vector3<int>(x, y, z);
				
				float threshold = 1;
				//Intentar que com més abaix + (molt més) probable que sigui sòlid
				float density = getDensity(bio, cPos * CHUNKSIZE + pos);

				if (transition) {
					int range = 8;
					float index = 0;
					Vector3<int> bpos = cPos * CHUNKSIZE + pos;
					for (int nX = bpos.x - range; nX <= bpos.x + range; nX++) {
						for (int nZ = bpos.z - range; nZ <= bpos.z + range; nZ++) {
							Vector3<int> nbPos = Vector3<int>(nX, cPos.y, nZ);
							Bioma nBio = getBiomeAt(nX / CHUNKSIZE, nZ / CHUNKSIZE);
							//float dist = Vector3<int>::module((cPos*CHUNKSIZE + pos) - (ncPos * CHUNKSIZE + Vector3(CHUNKSIZE/2, CHUNKSIZE/2, CHUNKSIZE/2)));
							//dist = std::min(dist/CHUNKSIZE, 1.0f);
							//dist /= CHUNKSIZE;
							//dist = abs(nX - cPos.x);
							//printf("density %f threshold %f\n", density, threshold);
							//threshold += getThreshold(nBio);
							density += getDensity(nBio, bpos);// / dist;
							threshold += 1.0f;// / dist;
							index++;
						}
					}
				}
			
				//printf("density %f threshold %f\n", density, threshold);
				if (density < threshold) {
					if (CHUNKSIZE * cPos.y + y > sealvl) {
						if (bio == Bioma::MAR || bio == Bioma::OCEA) {
							//continue;
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