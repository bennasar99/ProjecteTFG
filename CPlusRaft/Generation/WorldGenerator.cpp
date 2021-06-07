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
	this->oceanNoise.SetFrequency(0.001f / biomeSize); 
	
	//Mars interiors
	this->seaNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->seaNoise.SetFrequency(0.001f);
	this->seaNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
	//this->riverNoise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Hybrid);
	//this->riverNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Div);

	//Rius
	this->riverNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->riverNoise.SetFrequency(0.02f * riverProb);
	this->riverNoise.SetFractalOctaves(1);
	this->riverNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);

	//Clima (calor, templat, fred)
	this->climateNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->climateNoise.SetFrequency(0.001f / biomeSize); //0.01
	//this->climateNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);

	//Bioma (depen del clima)
	this->biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	this->biomeNoise.SetFrequency(0.004f / biomeSize); //0.1
	this->biomeNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);

	//Coves
	this->caveNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	this->caveNoise.SetFrequency(0.1f * caveProb); //0.01
	this->caveNoise.SetFractalOctaves(3);
	this->caveNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
	//this->caveNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2);

	//Ores
	this->oreNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->oreNoise.SetFrequency(1.0f * oreProb); //0.01
	//this->oreNoise.SetFractalOctaves(3);
	//this->oreNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);

	//Densitat biomes
	this->normalNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->oceanGenNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->mountainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	this->normalNoise.SetFrequency(0.01f); //0.01
	this->mountainNoise.SetFrequency(0.02f);
	this->oceanGenNoise.SetFrequency(0.002f);

	this->world = world;
}

WorldGenerator::WorldGenerator() {
	this->biomeSize = 1;
	this->oceanProb = 0.1f;
	this->seaToOcean = 0.1f;
	this->climHotProb = 0.1f;
	this->climWarmProb = 0.1f;
	this->climColdProb = 0.1f;
	this->caveSize = 0.1f;
	this->caveProb = 0.1f;
	this->oreProb = 0.1f;
	this->oreSize = 0.1f;
	this->riverProb = 0.1f;
	this->riverSize = 0.1f;
	this->seed = 0;
	this->world = nullptr;
}

Bioma WorldGenerator::getBiomeAt(int bX, int bZ) {
	float ocean = oceanNoise.GetNoise((float)bX, (float)bZ);

	float seaThreshold = -0.75f + this->oceanProb * 1.5f; //1.5 perquè és el rang del renou -0,75|0,75 (aprox)
	float oceanThreshold = seaThreshold - this->seaToOcean*(seaThreshold + 0.75f);
	if (ocean < oceanThreshold) {
		return Bioma::OCEA;
	}
	else if (ocean < seaThreshold) {
		return Bioma::MAR;
	}

	float sea = seaNoise.GetNoise((float)bX, (float)bZ);

	float climate = climateNoise.GetNoise((float)bX, (float)bZ);
	float warmThreshold = -0.75f + this->climColdProb * 1.5f;
	float transition = 0.1f;
	if (sea > 0.5f) {
		if (climate < (warmThreshold - transition)) {
			return Bioma::GEL;
		}
		return Bioma::MAR;
	}

	float river = riverNoise.GetNoise((float)bX, (float)bZ);
	float riverSize = this->riverSize * this->riverProb; //Ja que hem multiplicat la freqüència per aquest valor, si volem mantenir la mida de riu constant també ho hem de fer aquí
	if (river > (1 - riverSize)) {
		if (climate < (warmThreshold - transition)) { //Riu congelat
			return Bioma::GEL;
		}
		return Bioma::MAR;
	}

	float hotThreshold = warmThreshold + this->climWarmProb * 1.5f;
	if (climate < (warmThreshold - transition)) { //Clima fred
		return Bioma::ARTIC;
	}
	else if (climate >= (warmThreshold - transition) && climate < (warmThreshold + transition)) {
		return Bioma::BOSCNEVAT;
	}
	else if (climate >= (warmThreshold + transition) && climate < (hotThreshold - transition)) { //Clima templat
		float biome = biomeNoise.GetNoise((float)bX, (float)bZ);
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
	else if (climate >= (hotThreshold - transition) && (climate < (hotThreshold + transition))) {
		return Bioma::SABANA;
	}
	else if (climate >= (hotThreshold + transition)) { //Clima calent
		return Bioma::DESERT;
	}
	return Bioma::OCEA;
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
	case Bioma::BOSCNEVAT:
		randmax = 60;
		break;
	case Bioma::SABANA:
		randmax = 80;
		break;
	case Bioma::PLANA:
		randmax = 100;
		break;
	}

	if (randmax <= 0) {
		chunk->updateMesh();
		return chunk;
	}

	std::default_random_engine rng{ (unsigned int)((long)((chunk->getPos().x + chunk->getPos().y + chunk->getPos().z + static_cast<int>(chunk->getBiome()))* this->seed) % UINT_MAX ) };
	std::uniform_int_distribution<> dist{ 1, randmax };

	Vector3<int> pos = Vector3<int>(0, 0, 0);
	for (pos.x = 0; pos.x < CHUNKSIZE; pos.x++) {
		for (pos.z = 0; pos.z < CHUNKSIZE; pos.z++) {
			//Agafam la coordenada Y amb un bloc i aire damunt més gran
			bool trobat = false;
			for (pos.y = CHUNKSIZE-1; (pos.y >= 0)/*&&(!trobat)*/; pos.y--) {
				Vector3<int> tpos = cPos * CHUNKSIZE + pos + Vector3<int>(0, 1, 0);
				Bloc b1 = chunk->getBlock(pos);
				Bloc b2 = chunk->getBlockWorld(tpos);
				if (b1 == Bloc::TERRA && (b2 == Bloc::RES || b2 == Bloc::NEUSUP)){
					trobat = true;
					int random = dist(rng);
					if (random == 4 || random == 5 || random == 6 || random == 7 || random == 8) {
						//chunk->setBlock(new SpreadBlock(Bloc::HERBA, tpos), tpos);
						world->setBlock(Bloc::HERBA, tpos, false, false);
						if (random == 5 || random == 7) {
							world->setBlock(Bloc::HERBA, tpos + Vector3<int>(0, 1, 0), false, false);
						}
						
					}
					else if (random == 9) {
						//Tronc
						//chunk->setBlock(new SolidBlock(Bloc::FUSTAARBRE), tpos);
						world->setBlock(Bloc::FUSTAARBRE, tpos, false, false);
						std::uniform_int_distribution<> dist2{ 1, 5 };
						int rand2 = dist2(rng) + 1;
						for (int i = 1; i <= rand2; i++) {
							Vector3<int> fpos = tpos + Vector3<int>(0, i, 0);
							world->setBlock(Bloc::FUSTAARBRE, fpos, false, false);
						}
						//Fulles
						int altura = 1;
						if (bio == Bioma::BOSCNEVAT) {
							altura = 3;
						}
						if (bio != Bioma::SABANA) {
							std::uniform_int_distribution<> dist3{ 1, 3 };
							altura += dist3(rng);
						}
						std::uniform_int_distribution<> dist4{ 1, rand2 };
						int amplada = dist4(rng) + 1;
						for (int y = 1; y <= altura; y++) {
							for (int x = -amplada; x <= amplada; x++) {
								for (int z = -amplada; z <= amplada; z++) {
									Vector3<int> lpos = tpos + Vector3<int>(0, rand2 + y, 0)
										+ Vector3<int>(x, 0, 0) + Vector3<int>(0, 0, z);
									world->setBlock(Bloc::FULLAARBRE, lpos, false, false);
									if (bio == Bioma::BOSCNEVAT && (abs(x) == amplada || abs(z) == amplada || y == altura) && world->getBlock(lpos + Vector3<int>(0, 1, 0)) == Bloc::RES) {
										world->setBlock(Bloc::NEUSUP, lpos+Vector3<int>(0,1,0), false, false);
									}
								}
							}
							if (bio == Bioma::BOSCNEVAT) {
								amplada--;
							}
						}
					}
					else if (random == 10) { //Possible mob
						std::uniform_int_distribution<> dist5{ 1, 20 };
						int rand2 = dist5(rng);
						if (rand2 == 5) {
							world->addEntity(Entitat::OVELLA, Vector3<float>((float)tpos.x, (float)tpos.y + 1.0f, (float)tpos.z));
						}
						else if (rand2 == 7) {
							world->addEntity(Entitat::ESTRUC, Vector3<float>((float)tpos.x, (float)(tpos.y + 1), (float)tpos.z));
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
		return (pos.y / terlvl + noise->GetNoise((float)pos.x, (float)pos.y, (float)pos.z)/8.0f);
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
				else {
					density += getDensity(bio, bpos);// / dist;
					threshold += 1.0f;// / dist;
				}
			
				float fdensity = density / threshold;
				//printf("density %f threshold %f\n", density, threshold);
				if (fdensity < 1) {
					float prob0 = ((float)(sealvl) / (float)(abs(bpos.y+1)));
					prob0 = std::max(0.5f, prob0);
					prob0 = std::min(1.5f, prob0);
					float prob = 1 - caveSize*caveProb*prob0; //Així és menys probable trobar coves a dalt

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
					else if (bio == Bioma::BOSCNEVAT) {
						if (fdensity <= 0.95f) {
							toSet = Bloc::PEDRA;
						}
						else if (fdensity <= 0.995f) {
							toSet = Bloc::TERRA;
						}
						else if (fdensity < 1.0f){
							toSet = Bloc::NEUSUP;
						}
					}
					else if (bio == Bioma::SABANA) {
						if (fdensity <= 0.95f) {
							toSet = Bloc::PEDRA;
						}
						else if (fdensity <= 0.995f) {
							toSet = Bloc::TERRA;
						}
						else if (fdensity < 1.0f) {
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
					if (toSet == Bloc::PEDRA) {
						float heightMod = (float)sealvl / (float)(abs(bpos.y));
						heightMod = std::min(heightMod, 1.2f);
						float oreProb = this->oreSize * this->oreProb * heightMod; // + probable com + abaix
						if (oreNoise.GetNoise((float)bpos.x, (float)bpos.y, (float)bpos.z) > (0.72f - oreProb)) {
							Bloc ores[3] = { Bloc::OR, Bloc::FERRO, Bloc::CARBO };
							std::default_random_engine rng{ (unsigned int)(cPos.x + cPos.y + cPos.z + static_cast<int>(bio)) };
							std::uniform_int_distribution<> dist{ 1, 3 };
							toSet = ores[dist(rng) % 3];
						}
					}
					chunk->setBlock(toSet, pos);
				}
				else {
					if ((CHUNKSIZE*cPos.y + y) <= sealvl) {
						if ((CHUNKSIZE * cPos.y + y) == sealvl && (bio == Bioma::ARTIC || bio == Bioma::GEL)) {
							chunk->setBlock(Bloc::GEL, pos);
							if (rand() % (CHUNKSIZE*CHUNKSIZE) == 0) {
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

	this->oceanNoise.SetSeed(seed * 1);
	this->climateNoise.SetSeed(seed * 2);
	this->biomeNoise.SetSeed(seed * 3);
	this->caveNoise.SetSeed(seed * 4);
	this->normalNoise.SetSeed(seed*5);
	this->mountainNoise.SetSeed(seed*6);
	this->oceanGenNoise.SetSeed(seed*7);
	this->riverNoise.SetSeed(seed*8);
	this->seaNoise.SetSeed(seed*9);
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
	ryml::read(gen["ore_size"], &this->oreSize);
	ryml::read(gen["river_prob"], &this->riverProb);
	ryml::read(gen["river_size"], &this->riverSize);
	info.close();
}