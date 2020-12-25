#include "World.h"


World::World(int seed, int sizex, int sizey, int sizez, Camera* camera) //Nou
{
	this->wGen = WorldGenerator(seed, this);
	this->br = new BlockRenderer();

	this->noise.SetNoiseType(FastNoiseLite::NoiseType_Value);
	this->noise.SetFrequency(0.01f);
	this->noise.SetFractalGain(0.8f);
	this->noise.SetSeed(seed);

	this->seed = seed;
	this->camera = camera;
	this->minpos = Vector3<int>(sizex - 1, sizey - 1, sizez - 1);

	//NOU CODI CHUNKS:
	this->chunks = new Chunk * [(size_t)sizex * (size_t)sizey * (size_t)sizez];
	for (int i = 0; i < sizex * sizey * sizez; i++) {
		this->chunks[i] = nullptr;
	}

	this->sizex = sizex;
	this->sizey = sizey;
	this->sizez = sizez;

	this->generate(seed);

	//Col�locam c�mera
	int x = rand() % (sizex * CHUNKSIZE);
	int z = rand() % (sizez * CHUNKSIZE);
	for (int y = this->sizey * CHUNKSIZE; y > 0; y--) {
		if (getBlock(Vector3<int>(x, y, z)) == Bloc::TERRA) {
			spawn = Vector3<int>(x, y + 2, z);
			break;
		}
	}
}

World::World(std::string name, Camera* camera) { //C�rrega ja existent
	this->br = new BlockRenderer();
	this->seed = 0;

	this->noise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
	this->noise.SetFrequency(0.01f);
	this->noise.SetFractalGain(0.8f);
	this->noise.SetSeed(seed);

	this->camera = camera;
	this->minpos = Vector3<int>(sizex - 1, sizey - 1, sizez - 1);

	//Lectura informaci� m�n
	std::ifstream info("worlds/" + name + "/info.yml");
	std::string str((std::istreambuf_iterator<char>(info)),
		std::istreambuf_iterator<char>());
	char *par = (char*)str.c_str();
	c4::substr parse = c4::to_substr(par);
	ryml::Tree tree = ryml::parse(parse);
	ryml::read(tree["seed"], &this->seed);
	ryml::read(tree["spawn"]["x"], &this->spawn.x);
	ryml::read(tree["spawn"]["y"], &this->spawn.y);
	ryml::read(tree["spawn"]["z"], &this->spawn.z);
	ryml::read(tree["size"]["x"], &this->sizex);
	ryml::read(tree["size"]["y"], &this->sizey);
	ryml::read(tree["size"]["z"], &this->sizez);
	printf("Mides del mon: %d %d %d, spawn a: %d %d %d. Seed %d\n", this->sizex, this->sizey, this->sizez, this->spawn.x, this->spawn.y, this->spawn.z, this->seed);
	info.close();

	std::fstream file;

	this->wGen = WorldGenerator(this->seed, this);

	file.open("worlds/" + name + "/chunks.cnk", std::ios::in | std::ios::binary);

	this->chunks = new Chunk * [(size_t)sizex * (size_t)sizey * (size_t)sizez];

	//printf("Mides del mon: %d %d %d, spawn a: %d %d %d. Seed %d\n", this->sizex, this->sizey, this->sizez, sX, sY, sZ, this->seed);

	const int chunkSize = CHUNKSIZE * CHUNKSIZE * CHUNKSIZE;
	char buffer[chunkSize];
	char zeros[chunkSize];
	memset(zeros, 0, chunkSize);
	for (int x = 0; x < this->sizex; x++) {
		for (int y = 0; y < this->sizey; y++) {
			for (int z = 0; z < this->sizez; z++) {
				//printf("Chunk a %d %d %d...\n", x, y, z);
				int desp = getDesp(Vector3<int>(x, y, z));
				file.read(buffer, chunkSize);
				if (memcmp(buffer, zeros, chunkSize) != 0) { //Si el chunk no �s buit, el carregam
					chunks[desp] = new Chunk(this, Vector3<int>(x, y, z));
					chunks[desp]->readFromByteData(buffer);
				}
				//printf("LLEGIT\n");
			}
		}
	}
	printf("M�n llegit");
	fflush(stdout);;

	file.close();

	//Fi generaci� m�n
	for (int i = 0; i < this->sizex * this->sizey * this->sizez; i++) {
		if (chunks[i] != nullptr) {
			//chunks[i]->updateDL();
			//chunks[i]->updateMesh();
		}
	}

	//std::ifstream file("worlds / " + name + " / info.yml");
}

void World::save(std::string name) {
	
	std::string path = "worlds/" + name;
	std::filesystem::create_directory(path);

	std::fstream file;
	file.open("worlds/" + name + "/chunks.cnk", std::ios::out | std::ios::binary);

	const int chunkSize = CHUNKSIZE* CHUNKSIZE* CHUNKSIZE;
	char buffer[chunkSize];

	for (int x = 0; x < this->sizex; x++) {
		for (int y = 0; y < this->sizey; y++) {
			for (int z = 0; z < this->sizez; z++) {
				int desp = getDesp(Vector3<int>(x, y, z));
				if (chunks[desp] != nullptr) {
					chunks[desp]->getByteData(buffer);
					file.write(buffer, chunkSize);
				}
				else {
					char zeros[chunkSize];
					memset(zeros, 0, chunkSize);
					file.write(zeros, chunkSize);
				}
			}
		}
	}

	file.close();

	//Cream yml del m�n
	std::ofstream info("worlds/" + name + "/info.yml");
	info << "seed: " << seed << "\n";
	info << "spawn:\n";
	info << " x: " << this->spawn.x << "\n";
	info << " y: " << this->spawn.y << "\n";
	info << " z: " << this->spawn.z << "\n";
	info << "size:\n";
	info << " x: " << this->sizex << "\n";
	info << " y: " << this->sizey << "\n";
	info << " z: " << this->sizez << "\n";
	info.close();
}

void World::generate(int seed) {
	//Generador del m�n
	srand(seed); //Seed? xD
	noise.SetSeed(seed);
	Vector3<int> pos = Vector3<int>(0, 0, 0);
	//int sealvl = 100;//(int)floorf(((float)this->sizey * CHUNKSIZE) / 2.0f);
	//float lasty = 0; // = (this->sizey * CHUNKSIZE) / 2.0f;
	//for (pos.x = 0; pos.x < (this->sizex * CHUNKSIZE); pos.x++) {
	//	for (pos.z = 0; pos.z < (this->sizez * CHUNKSIZE); pos.z++) {
	//		//printf("gen %d %d %d\n", pos.x, pos.z);
	//		lasty = sealvl + noise.GetNoise((float)pos.x, (float)pos.z) * ((this->sizey*CHUNKSIZE)/3);
	//		lasty = std::min(lasty, (float)this->sizey * CHUNKSIZE); //No ha de superar l'altura del m�n
	//		lasty = std::max(lasty, 0.0f); // Ni ser menor que 0
	//		//printf("last: %f\n", lasty);
	//		for (pos.y = 0; pos.y < lasty; pos.y++) {
	//			//("terra %d %d %d\n", pos.x, pos.z);
	//			this->setBlock(Bloc::TERRA, pos, nullptr, false);
	//			if (pos.y == (int)lasty) {
	//				if (lasty > sealvl) { //Elements superf�cie
	//					int random = rand() % 128;
	//					if (random == 1 || random == 5) {
	//						this->setBlock(Bloc::HERBA, pos + Vector3<int>(0, 1, 0), 0, false);
	//					}
	//					else if (random == 2 || random == 6) {
	//						this->setBlock(Bloc::HERBAFULL, pos + Vector3<int>(0, 1, 0), 0, false);
	//						this->setBlock(Bloc::HERBA, pos + Vector3<int>(0, 2, 0), 0, false);
	//					}
	//					else if (random == 3) {
	//						this->setBlock(Bloc::AIGUA, pos, 0, false);
	//					}
	//					else if (random == 4) {
	//						//Tronc
	//						this->setBlock(Bloc::FUSTAARBRE, pos, 0, false);
	//						int rand2 = rand() % 5 + 1;
	//						for (int i = 1; i <= rand2; i++) {
	//							this->setBlock(Bloc::FUSTAARBRE, pos + Vector3<int>(0, i, 0), 0, false);
	//						}
	//						//Fulles
	//						int altura = rand() % 3 + 1;
	//						int amplada = (rand() % (rand2)) + 1;
	//						for (int y = 0; y < altura; y++) {
	//							for (int x = -amplada; x <= amplada; x++) {
	//								for (int z = -amplada; z <= amplada; z++) {
	//									Vector3<int> lpos = pos + Vector3<int>(0, rand2 + y, 0)
	//										+ Vector3<int>(x, 0, 0) + Vector3<int>(0, 0, z);
	//									if (lpos.z >= 0 && lpos.z < this->sizez * CHUNKSIZE && lpos.x >= 0 && lpos.x < this->sizex * CHUNKSIZE) {
	//										this->setBlock(Bloc::FULLAARBRE, lpos, 0, false);
	//									}
	//								}
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//		//Oceans
	//		for (pos.y = (int)lasty; pos.y <= sealvl; pos.y++) {
	//			this->setBlock(Bloc::AIGUA, pos, nullptr, false);
	//		}
	//		if (lasty <= sealvl+1) {
	//			pos.y = sealvl + 1;
	//			this->setBlock(Bloc::AIGUA, pos, nullptr, false);
	//		}
	//	}
	//}

	for (pos.x = 0; pos.x < this->sizex; pos.x++) {
		for (pos.y = 0; pos.y < this->sizey; pos.y++) {
			for (pos.z = 0; pos.z < this->sizez; pos.z++) {
				chunks[getDesp(Vector3<int>(pos.x,pos.y,pos.z))] = wGen.generate(pos.x, pos.y, pos.z);
			}
		}
	}

}

//Actualitzam els llums del m�n
void World::updateLights(Vector3<float> camPos, Vector3<float> front, float camFovY, float aspect) {
	std::list<Light*>::iterator it;
	unsigned int currentLight = GL_LIGHT2;
	for (unsigned int i = GL_LIGHT2; i <= GL_LIGHT7; i++) {
		glDisable(i);
	}
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	std::list <Light*> segonapas;
	for (it = lights.begin(); (it != lights.end() && currentLight <= GL_LIGHT7); it++) {
		if ((*it)->getEnabled()) {
			if ((camera->isVisible((*it)->getPosVec(), 0)) || ((*it)->getDist() <= 1.0f)) { //Mostram nom�s les llums visibles o molt properes
				setLight(currentLight, *it);
				currentLight++;
			}
			else {
				segonapas.push_back(*it); //Ho posam darrera per despr�s ja tenir-ho ordenat per dist�ncia
			}
		}
	}

	//2a passada, nom�s tenim en compte la dist�ncia
	for (it = segonapas.begin(); (it != segonapas.end() && currentLight <= GL_LIGHT7); it++) { 
		setLight(currentLight, *it);
		currentLight++;
	}
	glPopMatrix();

	//Sol, establim la seva posici� segons el moment del dia
	this->solpos = Vector3<float>(cosf((daytime / 43200.0f) * 3.1416f), sinf((daytime / 43200.0f) * 3.1416f), 0.0f);
	GLfloat spos[4] = { solpos.x, solpos.y, solpos.z, 0.0f };
	glLightfv(sol, GL_POSITION, spos);
}

//Establim les propietats d'una llum determinada
void World::setLight(int lightnum, Light* light) {
	glLightfv(lightnum, GL_POSITION, light->getPos());
	glLightfv(lightnum, GL_DIFFUSE, light->getColor());
	glLightfv(lightnum, GL_SPECULAR, light->getSpecular());
	glLightfv(lightnum, GL_SPOT_DIRECTION, light->getDir());
	glLightf(lightnum, GL_CONSTANT_ATTENUATION, light->getAttenuation(Attenuation::CONSTANT));
	glLightf(lightnum, GL_LINEAR_ATTENUATION, light->getAttenuation(Attenuation::LINEAR));
	glLightf(lightnum, GL_QUADRATIC_ATTENUATION, light->getAttenuation(Attenuation::QUADRATIC));
	glLightf(lightnum, GL_SPOT_EXPONENT, light->getLightConcentration());
	glLightf(lightnum, GL_SPOT_CUTOFF, light->getSpreadAngle());
	glEnable(lightnum);
}

//Afegeix un llum al m�n
Light* World::addLight(Vector3<float> pos) {
	Light* llum = new Light(pos);
	this->lights.push_front(llum);
	return llum;
}

//Elimina una llum del m�n
void World::delLight(Light* light) {
	this->lights.remove(light);
	delete light;
}

//Comparador per la llista de llums
static bool compareLights(Light* a, Light* b) {
	return a->getDist() < b->getDist();
}

/**
 * Actualitzaci� del m�n a cada frame (Idle)
 * @param delta temps des de la darrera actualitzaci� en ms
 * @param Vector3<float> posicio
 * Actualitza l'estat intern de les estructures del m�n, a m�s de la dist�ncia de les llums
 */
void World::update(int delta, Vector3<float> pos) {

	daytime += ((float)delta);
	if (daytime >= 86400.0f) {
		daytime = 0; //Reiniciam el temps del dia
		glEnable(sol); //Tornam activar el sol
	}
	else if (daytime > 43200.0f) { //Si es de vespre, desactivam el sol
		glDisable(sol);
	}
	
	//updTimer -= delta;
	//if (updTimer > 0) {
	//	return;
	//}
	//updTimer = 1000; //10 tps
	//Ordenaci� llums
	std::list<Light*>::iterator it;
	for (it = lights.begin(); (it != lights.end()); it++) {
		(*it)->setDist(Vector3<float>::module(pos - (*it)->getPosVec())); //Actualitzam la dist�ncia de les llums
	}
	lights.sort(compareLights); //Ordenam les llums per dist�ncia

	//NOU CODI CHUNKS
	pos = pos / CHUNKSIZE;
	pos.floor();
	int dist = floor(camera->getViewDist() / CHUNKSIZE / 3);
	for (int x = (int)(pos.x - dist); x < pos.x + dist; x++) {
		for (int y = (int)(pos.y - dist); y < pos.y + dist; y++) {
			for (int z = (int)(pos.z - dist); z < pos.z + dist; z++) {
				int desp = getDesp(Vector3<int>(x, y, z));
				if (desp != -1 && chunks[desp] != 0) {
					chunks[desp]->update(delta);
				}
			}
		}
	}

	//Actualitzam les entitats
	std::list<Entity*>::iterator ent;
	for (ent = entities.begin(); (ent != entities.end()); ent++) {
		(*ent)->update(delta);
	}
}

//Alliberam la mem�ria de tots els blocs i entitats
void World::destroy() {

	//NOU CODI CHUNKS
	for (int i = 0; i < this->sizex * this->sizey * this->sizez; i++) {
		this->chunks[i]->destroy();
	}

	std::list<Entity*>::iterator ent;
	for (ent = entities.begin(); (ent != entities.end()); ent++) {
		(*ent)->destroy();
		delete *ent;
	}
}

//Assignam un llum (GL_LIGHT0-7) al sol
void World::setSol(int sol) {
	this->sol = sol;
	GLfloat soldiff[4] = { 1, 1, 1, 1.0f };
	glLightfv(sol, GL_DIFFUSE, soldiff);
	glLightf(sol, GL_SPOT_CUTOFF, 180.0f);
	glLightf(sol, GL_SPOT_EXPONENT, 0.0f);
	glEnable(sol); //pensa a tornar posar Enable
}

//Col�locam un bloc a la posici� indicada
bool World::setBlock(Bloc tipus, Vector3<int> pos) {
	return this->setBlock(tipus, pos, 0, true);
}

//Eliminam el bloc de la posici� indicada
bool World::deleteBlock(Vector3<int> pos, bool destroy) { //Eliminar Bloc::RES?

	//pos.floor();
	Vector3<int> cpos = pos / CHUNKSIZE;
	//cpos.floor();
	Vector3<int> bpos = pos % CHUNKSIZE;
	int desp = getDesp(cpos);
	if (desp == -1) {
		return false;
	}
	return this->chunks[desp]->delBlock(bpos, destroy);
}

//Col�locam un bloc d'un tipus determinat a la posici� indicada
bool World::setBlock(Bloc tipus, Vector3<int> pos, Block* parent, bool listUpdate) {

	//NOU CODI CHUNKS:
	//pos.floor();
	Vector3<int> cpos = pos / CHUNKSIZE;
	//cpos.floor();
	Vector3<int> bpos = pos % CHUNKSIZE;
	int desp = getDesp(cpos);
	if (desp == -1) {
		return false;
	}

	if (chunks[desp] == 0){
		chunks[desp] = new Chunk(this, cpos);
	}

	Block* bloc;
	switch (tipus) {
	case Bloc::LLUMSOTIL: case Bloc::LLUMTERRA: case Bloc::TORXA:
		bloc = new LightBlock(this, tipus, pos);
		break;
	case Bloc::HERBA: case Bloc::HERBAFULL:
		bloc = new SpreadBlock(tipus, pos);
		break;
	case Bloc::ALTAVEU:
		bloc = new Jukebox(pos);
		break;
	case Bloc::AIGUA:
		bloc = new LiquidBlock(tipus, pos);
		break;
	default:
		bloc = new SolidBlock(tipus);
	}
	chunks[desp]->setBlock(bloc, bpos);
	if (listUpdate) {
		//chunks[desp]->updateDL();
		chunks[desp]->updateMesh();
		updateNeighborChunks(cpos, bpos);
	}

	//Actualitzam maxpos i minpos
	maxpos.x = std::max(maxpos.x, pos.x); minpos.x = std::min(minpos.x, pos.x);
	maxpos.y = std::max(maxpos.y, pos.y); minpos.y = std::min(minpos.y, pos.y);
	maxpos.z = std::max(maxpos.z, pos.z); minpos.z = std::min(minpos.z, pos.z);

	return true;
}

//Assignam un bloc (per punter) a la posici� indicada
bool World::setBlock(Block* bloc, Vector3<int> pos, bool listUpdate) {
	//pos.floor();
	Vector3<int> cpos = pos / CHUNKSIZE;
	//cpos.floor();
	Vector3<int> bpos = pos % CHUNKSIZE;
	int desp = getDesp(cpos);
	if (desp == -1) {
		return false;
	}

	//Actualitzam maxpos i minpos
	maxpos.x = std::max(maxpos.x, cpos.x); minpos.x = std::min(minpos.x, cpos.x);
	maxpos.y = std::max(maxpos.y, cpos.y); minpos.y = std::min(minpos.y, cpos.y);
	maxpos.z = std::max(maxpos.z, cpos.z); minpos.z = std::min(minpos.z, cpos.z);

	return chunks[desp]->setBlock(bloc, bpos);

	if (listUpdate) {
		//chunks[desp]->updateDL();
		chunks[desp]->updateMesh();
		updateNeighborChunks(cpos, bpos);
	}
}

//Obtenim el bloc a la posici� indicada
Bloc World::getBlock(Vector3<float> pos) {
	pos.floor();
	return getBlock(Vector3<int>((int)pos.x, (int)pos.y, (int)pos.z));
}

Bloc World::getBlock(Vector3<int> pos) {
	Vector3<int> cpos = pos / CHUNKSIZE;
	//cpos.floor();
	Vector3<int> bpos = Vector3<int>(pos.x % CHUNKSIZE, pos.y % CHUNKSIZE, pos.z % CHUNKSIZE);
	int desp = getDesp(cpos);
	if (desp == -1) {
		return Bloc::LIMIT;
	}
	if (chunks[desp] == 0) {
		return Bloc::RES;
	}
	return chunks[desp]->getBlock(bpos);
}

//remove indica si el bloc s'ha d'eliminar del m�n (NO S'ALLIBERA LA MEM�RIA)
Block* World::getBlockPointer(Vector3<int> pos, bool remove) {
	//pos.noDecimals();
	//Block* block = 0;
	//int desp = getDesp(pos);
	//if (desp != -1) {
	//	printf("%f %f %f, %d\n", pos.x, pos.y, pos.z, desp);
	//	if (blocs[desp] != 0) {
	//		printf("HOLA\n");
	//		block = blocs[desp];
	//	}
	//	if (remove) {
	//		blocs[desp] = 0;
	//	}
	//}
	//return block;
	return nullptr;
}

//Dibuixa el m�n visible
void World::draw(Vector3<float> pos, float dist) {

	camera->setViewDist(dist);

	std::list<Entity*>::iterator ent;
	for (ent = entities.begin(); (ent != entities.end()); ent++) {
		Vector3<float> pos = (*ent)->getPos();
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		(*ent)->draw();
		glPopMatrix();
	}

	//Obtenim el volum de possible visibilitat de la c�mera
	int xmin = camera->xmin;	int xmax = camera->xmax;
	int ymin = camera->ymin;	int ymax = camera->ymax;
	int zmin = camera->zmin;	int zmax = camera->zmax;

	xmin = std::max(xmin, (int)minpos.x);	ymin = std::max(ymin, (int)minpos.y);	zmin = std::max(zmin, (int)minpos.z);
	xmax = std::min(xmax, (int)maxpos.x);	ymax = std::min(ymax, (int)maxpos.y);	zmax = std::min(zmax, (int)maxpos.z);

	xmin = std::max(xmin, 0);				ymin = std::max(ymin, 0);				zmin = std::max(zmin, 0);
	xmax = std::min(xmax, this->sizex* CHUNKSIZE);		ymax = std::min(ymax, this->sizey* CHUNKSIZE);		zmax = std::min(zmax, this->sizez* CHUNKSIZE);

	//NOU CODI CHUNKS
	Vector3<int> cMin = Vector3<int>(xmin, ymin, zmin);
	cMin = cMin / CHUNKSIZE;

	Vector3<int> cMax = Vector3<int>(xmax, ymax, zmax);
	cMax = cMax / CHUNKSIZE;

	//printf("%f %f %f, %f %f %f\n", cMin.x, cMin.y, cMin.z, cMax.x, cMax.y, cMax.z);
	int nchunks = 0;
	for (float x = (float)cMin.x; x <= (float)cMax.x; x++) { //Optimitzaci� possible: si els chunks circumdants no son visibles, aquest no ho �s
		for (float y = (float)cMin.y; y <= (float)cMax.y; y++) {
			for (float z = (float)cMin.z; z <= (float)cMax.z; z++) {
				int desp = getDesp(Vector3<int>((int)x, (int)y, (int)z));
				if (chunks[desp] == nullptr) {
					continue;
				}
				float dist = Vector3<float>::module(camera->getPos() - Vector3<float>(x * CHUNKSIZE + CHUNKSIZE/2, y * CHUNKSIZE + CHUNKSIZE/2, z * CHUNKSIZE + CHUNKSIZE/2));
				if ((dist < CHUNKSIZE) || (camera->isVisible(Vector3<float>(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100))){
					
					glPushMatrix();
					glTranslatef(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE);
					chunks[desp]->drawO();
					glPopMatrix();
					nchunks++;
				}
			}
		}
	}
	for (float x = (float)cMin.x; x <= (float)cMax.x; x++) { //Optimitzaci� possible: si els chunks circumdants no son visibles, aquest no ho �s
		for (float y = (float)cMin.y; y <= (float)cMax.y; y++) {
			for (float z = (float)cMin.z; z <= (float)cMax.z; z++) {
				int desp = getDesp(Vector3<int>((int)x, (int)y, (int)z));
				if (chunks[desp] == nullptr) {
					continue;
				}
				float dist = Vector3<float>::module(camera->getPos() - Vector3<float>(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE));
				if ((dist < 24) || (camera->isVisible(Vector3<float>(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3<float>(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100))) {

					glPushMatrix();
					glTranslatef(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE);
					chunks[desp]->drawT();
					glPopMatrix();
					nchunks++;
				}
			}
		}
	}
	//printf("Chunks: %d\n", nchunks);
}

//Dibuixa un bloc a una posici� determinada (Sense guardar-lo al m�n)
void World::drawBloc(Bloc tipus) {
	br->drawBloc(tipus);
}

//Dibuixa l'eix de coordenades a la posici� indicada
void World::drawAxis(Vector3<float> pos, float axisSize) { 
	glPushMatrix();
	glDisable(GL_LIGHTING); //Volem que es vegin tot i no haver-hi llum

	glTranslatef(pos.x, pos.y, pos.z);

	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(axisSize, 0, 0);
	glVertex3f(-axisSize, 0, 0);
	glEnd();

	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(0, axisSize, 0);
	glVertex3f(0, -axisSize, 0);
	glEnd();

	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(0, 0, axisSize);
	glVertex3f(0, 0, -axisSize);
	glEnd();

	glEnable(GL_LIGHTING);
	glPopMatrix();
}

//Dibuixa l'esfera del sol i modifica el color del cel
void World::drawSol(Vector3<float> pos, float dist) {
	if (daytime >= 0.0f && daytime <= 43200.0f) {
		glPushMatrix();

		//El sol no s'ha de veure afectat per la boira o l'il�luminaci�
		glDisable(GL_FOG);
		glDisable(GL_LIGHTING);
		glTranslatef(pos.x, pos.y, pos.z);
		glTranslatef(solpos.x * dist, solpos.y * dist, solpos.z);
		glColor3f(1, 1, 0);
		drawSphere(1, 5, 5);
		glEnable(GL_LIGHTING);
		glEnable(GL_FOG);
		
		//Establim el color del cel (+ blanc com + adalt sigui el sol)
		glClearColor(1-solpos.y/1.5f, solpos.y / 3.0f, solpos.y, 1);

		glPopMatrix();
	}
	else {
		glClearColor(0, 0, 0.15f,1);
	}
}

//Interacci� amb un bloc
void World::interact(Vector3<int> pos) {
	//pos.floor();
	Vector3<int> cpos = pos / CHUNKSIZE;
	//cpos.floor();
	Vector3<int> bpos = Vector3<int>(pos.x % CHUNKSIZE, pos.y % CHUNKSIZE, pos.z % CHUNKSIZE);
	int desp = getDesp(cpos);
	if (desp == -1) {
		return;
	}
	return chunks[desp]->interact(bpos);
}

//Comprova que una posici� �s valida i retorna el despla�ament corresponent a la posici�
int World::getDesp(Vector3<int> pos) {
	int desp = pos.x + this->sizex * (pos.y + this->sizey * pos.z);
	if ((desp >= (this->sizex * this->sizey * this->sizez))||(desp < 0) || pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= this->sizex || pos.y >= this->sizey || pos.z >= this->sizez) {
		return -1;
	}
	return desp;
}

//Afegeix una entitat al m�n
Entity* World::addEntity(Entitat ent, Vector3<float> pos) {
	Entity* entitat = 0;
	//switch (ent) {
		//Ficar entitat amb new a *entitat
	//}
	entities.push_front(entitat);
	return entitat;
}

//Elimina una entitat del m�n
void World::delEntity(Entity* ent) {
	ent->destroy();
	entities.remove(ent); //L'eliminam de la llista
	delete ent; //Alliberam la seva memoria
}

//Obt� l'entitat m�s propera a una posici� dins la dist�ncia (range) indicada, podent requerir que sigui controlable.
Entity* World::getNearestEntity(Vector3<float> pos, float range, bool controllable){
	Entity* entitat = 0;
	std::list<Entity*>::iterator ent;
	float currDist = range;
	for (ent = entities.begin(); (ent != entities.end()); ent++) {
		float dist = Vector3<float>::module((*ent)->getPos() - pos);
		if (controllable && !(*ent)->getControllable()) { //Si no �s controlable i ho ha de ser, passam a la seg�ent
			continue;
		}
		if (dist < currDist){
			entitat = (*ent);
			currDist = dist;
		}
	}
	return entitat;
}

void World::updateNeighborChunks(Vector3<int> cpos, Vector3<int> bpos) {
	int desp;
	Vector3<int> ncpos;
	if (bpos.x == 15) {
		ncpos = cpos + Vector3<int>(1, 0, 0);
	}
	else if (bpos.x == 0) {
		ncpos = cpos - Vector3<int>(1, 0, 0);
	}
	desp = getDesp(ncpos);
	if (desp != -1) {
		if (chunks[desp] == nullptr) {
			chunks[desp] = new Chunk(this, ncpos);
		}
		chunks[desp]->updateMesh();
	}
	if (bpos.y == 15) {
		ncpos = cpos + Vector3<int>(0, 1, 0);
	}
	else if (bpos.y == 0) {
		ncpos = cpos - Vector3<int>(0, 1, 0);
	}
	desp = getDesp(ncpos);
	if (desp != -1) {
		if (chunks[desp] == nullptr) {
			chunks[desp] = new Chunk(this, ncpos);
		}
		chunks[desp]->updateMesh();
	}
	if (bpos.z == 15) {
		ncpos = cpos + Vector3<int>(0, 0, 1);
	}
	else if (bpos.z == 0) {
		ncpos = cpos - Vector3<int>(0, 0, 1);
	}
	desp = getDesp(ncpos);
	if (desp != -1) {
		if (chunks[desp] == nullptr) {
			chunks[desp] = new Chunk(this, ncpos);
		}
		chunks[desp]->updateMesh();
	}
}

Vector3<int> World::getSpawn() {
	return this->spawn;
}