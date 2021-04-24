#include "WorldGenerator.h"
#include "../world.h"
#include "../Chunk.h"

WorldGenerator::WorldGenerator(int seed, World* world, Generator gen) {
	this->setSeed(seed);
	this->biomeSize = 2;

	srand(seed);

	this->readFromFile();

	//Oceans
	this->oceanNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->oceanNoise.SetFrequency(0.002f / biomeSize); //0.04
	
	//Mars interiors
	this->seaNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->seaNoise.SetFrequency(0.001f);
	this->seaNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
	//this->riverNoise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Hybrid);
	//this->riverNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Div);

	//Rius
	this->riverNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->riverNoise.SetFrequency(0.002f);
	this->riverNoise.SetFractalOctaves(1);
	this->riverNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);

	//Clima (calor, templat, fred)
	this->climateNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	this->climateNoise.SetFrequency(0.005f / biomeSize); //0.01
	this->climateNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);

	//Bioma (depen del clima)
	this->biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	this->biomeNoise.SetFrequency(0.005f / biomeSize); //0.1
	this->biomeNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);

	//Coves
	this->caveNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	this->caveNoise.SetFrequency(0.1f / caveSize); //0.01
	//this->caveNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2);

	this->normalNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->oceanGenNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->mountainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->normalNoise.SetFrequency(0.01f); //0.01
	this->mountainNoise.SetFrequency(0.02f);
	this->oceanGenNoise.SetFrequency(0.002f);

	this->world = world;
}

WorldGenerator::WorldGenerator() {
	this->seed = 0;
	this->world = nullptr;
}

Bioma WorldGenerator::getBiomeAt(int bX, int bZ) {
	float ocean = oceanNoise.GetNoise((float)bX, (float)bZ);
	float climate = climateNoise.GetNoise((float)bX, (float)bZ);
	float biome = biomeNoise.GetNoise((float)bX, (float)bZ);
	float sea = seaNoise.GetNoise((float)bX, (float)bZ);
	float river = riverNoise.GetNoise((float)bX, (float)bZ);

	float seaThreshold = -0.75f + this->oceanProb * 1.5f; //1.5 perquè és el rang del renou -0,75|0,75 (aprox)
	float oceanThreshold = seaThreshold - this->seaToOcean*(seaThreshold + 0.75f);
	if (ocean < oceanThreshold) {
		return Bioma::OCEA;
	}
	else if (ocean < seaThreshold) {
		return Bioma::MAR;
	}

	if (sea > 0.5f) {
		return Bioma::MAR;
	}
	if (river > 0.9f) {
		return Bioma::MAR;
	}

	float warmThreshold = -0.75f + this->climColdProb * 1.5f;
	float hotThreshold = warmThreshold + this->climWarmProb * 1.5f;
	if (climate < warmThreshold) { //Clima fred
		return Bioma::ARTIC;
	}
	else if (climate >= warmThreshold && climate < hotThreshold) { //Clima templat
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
	else if (climate >= hotThreshold) { //Clima calent
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

//TODO: pot petar l'execució si és descarrega el chunk quan ja s'ha entrat aquí
Chunk* WorldGenerator::generateDetail(Chunk* chunk) { //Estructures, els chunks dels voltants ja estan generats
	Vector3<int> cPos = chunk->getPos();
	float density = 0;
	int randmax = INT32_MAX; //Com més alt, - densitat de coses
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
			//Agafam la coordenada Y amb un bloc i aire damunt més gran
			bool trobat = false;
			for (pos.y = CHUNKSIZE-1; (pos.y >= 0)/*&&(!trobat)*/; pos.y--) {
				Vector3<int> tpos = cPos * CHUNKSIZE + pos + Vector3<int>(0, 1, 0);
				Bloc b1 = chunk->getBlock(pos);
				Bloc b2 = chunk->getBlockWorld(tpos);
				if (b1 == Bloc::TERRA && b2 == Bloc::RES){
					trobat = true;
					int random = rand() % randmax;
					if (random == 4 || random == 5 || random == 6 || random == 7 || random == 8) {
						//chunk->setBlock(new SpreadBlock(Bloc::HERBA, tpos), tpos);
						world->setBlock(Bloc::HERBA, tpos, false, false);
					}
					else if (random == 9) {
						//Tronc
						//chunk->setBlock(new SolidBlock(Bloc::FUSTAARBRE), tpos);
						world->setBlock(Bloc::FUSTAARBRE, tpos, false, false);
						int rand2 = rand() % 5 + 1;
						for (int i = 1; i <= rand2; i++) {
							Vector3<int> fpos = tpos + Vector3<int>(0, i, 0);
							world->setBlock(Bloc::FUSTAARBRE, fpos, false, false);
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
										world->setBlock(Bloc::FULLAARBRE, lpos, false, false);
									}
								}
							}
						}
					}
					else if (random == 10) { //Possible mob
						int rand2 = rand() % 10;
						if (rand2 == 5) {
							world->addEntity(Entitat::OVELLA, Vector3<float>((float)tpos.x, (float)tpos.y + 1.0f, (float)tpos.z));
						}
						else if (rand2 == 7) {
							//world->addEntity(Entitat::ESTRUC, Vector3<float>(tpos.x, tpos.y + 1, tpos.z));
						}
					}
				}
			}
		}
	}
	chunk->updateMesh();
	return chunk;
}

float WorldGenerator::getDensity(Bioma bio, Vector3<int> pos) {
	float terlvl = 100;
	FastNoiseLite* noise;
	switch(bio){
	case Bioma::MUNTANYA:
		terlvl = 140;
		noise = &this->mountainNoise;
		return (pos.y / terlvl + noise->GetNoise((float)pos.x, (float)pos.y, (float)pos.z));
	case Bioma::OCEA:
		terlvl = 50;
		noise = &this->oceanGenNoise;
		return (pos.y / terlvl + noise->GetNoise((float)pos.x, (float)pos.y, (float)pos.z) / 4.0f);
	case Bioma::MAR: case Bioma::GEL:
		terlvl = 80;
		noise = &this->oceanGenNoise;
		return (pos.y / terlvl + noise->GetNoise((float)pos.x, (float)pos.y, (float)pos.z)/3.0f);
	default:
		noise = &this->normalNoise;
		return (pos.y / terlvl + noise->GetNoise((float)pos.x, (float)pos.y, (float)pos.z)/4.0f);
	}

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

	chunk->setBiome(getBiomeAt(cPos.x * CHUNKSIZE + CHUNKSIZE/2, cPos.z * CHUNKSIZE + CHUNKSIZE / 2));
	Bioma bio = chunk->getBiome();

	bool transition = false;
	for (int nX = cPos.x - 1; (nX <= cPos.x + 1) && !transition; nX++) {
		for (int nZ = cPos.z - 1; (nZ <= cPos.z + 1) && !transition; nZ++) {
			if (getBiomeAt(nX * CHUNKSIZE + CHUNKSIZE / 2, nZ * CHUNKSIZE + CHUNKSIZE / 2) != bio) {
				transition = true;
			}
		}
	}
	
	int sealvl = 90;
	
	//Bioma bioN[8];

	bool block = false;
	int waterblocksup = 0;
	int nblocs = 0;
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int z = 0; z < CHUNKSIZE; z++) {

			if (transition) { //si és un chunk de transició entre biomes, cada bloc pot ser d'un bioma diferent
				bio = getBiomeAt(cPos.x * CHUNKSIZE + x, cPos.z * CHUNKSIZE + z);
			}

			//float height = 1;
			//if (!WorldGenerator::isBiome3D(bio)) {
			//	height = getHeight(bio, x + cPos.x * CHUNKSIZE, z + cPos.z * CHUNKSIZE);
			//	if (height < (cPos.y) * CHUNKSIZE && (cPos.y)*CHUNKSIZE > sealvl) { //No importa generar la columna
			//		continue;
			//	}
			//}


			for (int y = 0; y < CHUNKSIZE; y++) {

				Vector3<int> pos = Vector3<int>(x, y, z);
				Vector3<int> bpos = cPos * CHUNKSIZE + pos;
				float threshold = 0;
				//Intentar que com més abaix + (molt més) probable que sigui sòlid
				float density = 0;
				if (transition) {
					float bioDen[NBIOMES] = {};
					int range = 4;
					for (int nX = bpos.x - range; nX <= bpos.x + range; nX++) {
						for (int nZ = bpos.z - range; nZ <= bpos.z + range; nZ++) {
							Vector3<int> nbPos = Vector3<int>(nX, y, nZ);
							Bioma nBio = getBiomeAt(nX, nZ);
							float dist = Vector3<int>::module(bpos - nbPos);
							dist = std::max(1.0f, dist);
							int bI = static_cast<int>(nBio);
							if (bioDen[bI] == 0) {
								bioDen[bI] = getDensity(nBio, bpos);
							}
							density += bioDen[bI] / dist;
							threshold += 1.0f / dist;
						}
					}
				}
				//else/* if (!WorldGenerator::isBiome3D(bio))*/ {
				//	density += (float)bpos.y / height;
				//	threshold += 1.0f;
				//}
				else { //Bioma 3D
					density += getDensity(bio, bpos);// / dist;
					threshold += 1.0f;// / dist;
				}
			
				float fdensity = density / threshold;
				//printf("density %f threshold %f\n", density, threshold);
				if (fdensity < 1) {
					float prob0 = ((float)sealvl / (float)bpos.y);
					float prob = -caveProb * prob0;
					prob = std::min(prob, -0.5f);
					prob = std::max(prob, -0.3f);

					if (caveNoise.GetNoise((float)bpos.x, (float)bpos.y, (float)bpos.z) > prob) { //-0.4
						continue;
					}

					nblocs++;

					Bloc toSet;
					if (bio == Bioma::MUNTANYA) {
						toSet = Bloc::PEDRA;
					}
					else if (bio == Bioma::ARTIC) {
						if (fdensity <= 0.95f) {
							toSet = Bloc::PEDRA;
						}
						else if (fdensity <= 0.98f) {
							toSet = Bloc::TERRA;
						}
						else {
							toSet = Bloc::NEU;
						}
					}
					else if (bio == Bioma::DESERT) {
						if (fdensity <= 0.92f) {
							toSet = Bloc::PEDRA;
						}
						else {
							toSet = Bloc::ARENA;
						}
					}
					else {
						if (fdensity <= 0.95f) {
							toSet = Bloc::PEDRA;
						}
						else {
							toSet = Bloc::TERRA;
						}
					}

					//Minerals
					int threshold = 94 + (cPos.y);
					if (toSet == Bloc::PEDRA) {
						if (rand() % 100 > threshold) {
							Bloc ores[3] = { Bloc::OR, Bloc::FERRO, Bloc::CARBO };
							toSet = ores[rand() % 3];
						}
					}
					chunk->setBlock(toSet, pos);
				}
				else {
					if ((CHUNKSIZE*cPos.y + y) <= sealvl) {
						if ((CHUNKSIZE * cPos.y + y) == sealvl && bio == Bioma::ARTIC) {
							chunk->setBlock(Bloc::GEL, pos);
							if (rand() % CHUNKSIZE == 0) {
								chunk->setBlock(Bloc::AIGUA, pos);
							}
						}
						else {
							chunk->setBlock(Bloc::AIGUA, pos);
						}
						nblocs++;
					}
					/*else if (!WorldGenerator::isBiome3D(bio) && !transition) {
						y = CHUNKSIZE;
					}*/
				}
			}
		}
	}
	if (nblocs == 0) {
		//return nullptr;
	}
	return chunk;
}

bool WorldGenerator::isBiome3D(Bioma bio) {
	if (bio == Bioma::MUNTANYA /*|| bio == Bioma::ARTIC*/) { //Include list
		return true;
	}
	return false;
}

void WorldGenerator::setSeed(int seed) {
	this->seed = seed;

	this->oceanNoise.SetSeed(seed * 4);
	this->climateNoise.SetSeed(seed * 3);
	this->biomeNoise.SetSeed(seed * 2);
	this->caveNoise.SetSeed(seed * 5);
	this->normalNoise.SetSeed(seed);
	this->mountainNoise.SetSeed(seed);
	this->oceanGenNoise.SetSeed(seed);
	this->riverNoise.SetSeed(seed);
	this->seaNoise.SetSeed(seed);
}

int WorldGenerator::getSeed() {
	return this->seed;
}

void WorldGenerator::readFromFile() {
	std::string path = "settings/worldGen.yml";
	std::ifstream info(path.c_str());
	std::string str((std::istreambuf_iterator<char>(info)), std::istreambuf_iterator<char>());
	char* par = (char*)str.c_str();
	c4::substr parse = c4::to_substr(par);
	ryml::Tree tree = ryml::parse(parse);
	auto gen = tree["generators"]["normal"];
	ryml::read(gen["biome_size"], &this->biomeSize);
	ryml::read(gen["ocean_prob"], &this->oceanProb);
	ryml::read(gen["sea_to_ocean_ratio"], &this->seaToOcean);
	ryml::read(gen["biome_size"], &this->biomeSize);
	ryml::read(gen["climate_prob"]["hot"], &this->climHotProb);
	ryml::read(gen["climate_prob"]["warm"], &this->climWarmProb);
	ryml::read(gen["climate_prob"]["cold"], &this->climColdProb);
	ryml::read(gen["cave_size"], &this->caveSize);
	ryml::read(gen["cave_prob"], &this->caveProb);
	ryml::read(gen["ore_prob"], &this->oreProb);
	info.close();
}