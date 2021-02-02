#include "World.h"


World::World(std::string name, int seed, int sizex, int sizey, int sizez, Camera* camera) //Nou
{
	this->genCores = ThreadManager::getCoreCount() - 2;
	this->genCores = std::max(this->genCores, 1);

	this->cnk = std::vector< std::future<Chunk*> >(genCores);
	this->name = name;
	this->wGen = WorldGenerator(seed, this);
	this->br = new BlockRenderer();

	this->seed = seed;
	this->camera = camera;

	this->size.x = sizex;
	this->size.y = sizey;
	this->size.z = sizez;

	//NOU CODI CHUNKS:
	this->chunks = new Chunk * [(size_t)size.x * (size_t)size.y * (size_t)size.z];
	this->estat = new ChunkState[(size_t)size.x * (size_t)size.y * (size_t)size.z];
	for (int i = 0; i < size.x * size.y * size.z; i++) {
		this->chunks[i] = nullptr;
		this->estat[i] = ChunkState::BUIT;
	}

	//Col�locam c�mera
	int x = rand() % (size.x * CHUNKSIZE);
	int z = rand() % (size.z * CHUNKSIZE);
	bool trobat = false;
	for (int y = (this->size.y * CHUNKSIZE - 1); y > 0 && (!trobat); y--) {
		Bloc b = getBlock(Vector3<int>(x, y, z));
		if (b != Bloc::RES) {
			trobat = true;
			spawn = Vector3<int>(x, y + 2, z);
		}
	}
}

World::World(std::string name, Camera* camera) { //C�rrega ja existent

	this->br = new BlockRenderer();
	this->genCores = ThreadManager::getCoreCount() - 2;
	this->genCores = std::max(this->genCores, 1);

	this->seed = 0;
	this->name = name;

	this->camera = camera;
	this->minpos = Vector3<int>(size.x - 1, size.y - 1, size.z - 1);

	//Lectura informaci� m�n
	std::ifstream info("worlds/" + name + "/info.yml");
	std::string str((std::istreambuf_iterator<char>(info)), std::istreambuf_iterator<char>());
	char *par = (char*)str.c_str();
	c4::substr parse = c4::to_substr(par);
	ryml::Tree tree = ryml::parse(parse);
	ryml::read(tree["seed"], &this->seed);
	ryml::read(tree["spawn"]["x"], &this->spawn.x);
	ryml::read(tree["spawn"]["y"], &this->spawn.y);
	ryml::read(tree["spawn"]["z"], &this->spawn.z);
	ryml::read(tree["size"]["x"], &this->size.x);
	ryml::read(tree["size"]["y"], &this->size.y);
	ryml::read(tree["size"]["z"], &this->size.z);
	printf("Mides del mon: %d %d %d, spawn a: %d %d %d. Seed %d\n", this->size.x, this->size.y, this->size.z, this->spawn.x, this->spawn.y, this->spawn.z, this->seed);
	info.close();

	std::fstream file;
	this->cnk = std::vector< std::future<Chunk*> >(genCores);
	this->wGen = WorldGenerator(this->seed, this);
	this->chunks = new Chunk * [(size_t)size.x * (size_t)size.y * (size_t)size.z];
	this->estat = new ChunkState[(size_t)size.x * (size_t)size.y * (size_t)size.z];
	for (int i = 0; i < size.x * size.y * size.z; i++) {
		this->chunks[i] = nullptr;
		this->estat[i] = ChunkState::BUIT;
	}
	//Carregam les regions
	for (int rX = 0; rX < ceil((float)this->size.x / 16.0f); rX++) {
		for (int rY = 0; rY < ceil((float)this->size.y / 16.0f); rY++) {
			for (int rZ = 0; rZ < ceil((float)this->size.z / 16.0f); rZ++) {
				loadRegion(Vector3<int>(rX, rY, rZ));
			}
		}
	}

	//printf("Mides del mon: %d %d %d, spawn a: %d %d %d. Seed %d\n", this->size.x, this->size.y, this->size.z, sX, sY, sZ, this->seed);
}

void World::save() {
	
	std::string path = "worlds/" + this->name;
	std::filesystem::create_directory(path.c_str());
	path += "/chunks";
	std::filesystem::create_directory(path.c_str());

	for (int rX = 0; rX < ceil((float)this->size.x / 16.0f); rX++) {
		for (int rY = 0; rY < ceil((float)this->size.y / 16.0f); rY++) {
			for (int rZ = 0; rZ < ceil((float)this->size.z / 16.0f); rZ++) {
				saveRegion(Vector3<int>(rX, rY, rZ));
			}
		}
	}

	//Cream yml del m�n
	std::ofstream info("worlds/" + name + "/info.yml");
	info << "seed: " << seed << "\n";
	info << "spawn:\n";
	info << " x: " << this->spawn.x << "\n";
	info << " y: " << this->spawn.y << "\n";
	info << " z: " << this->spawn.z << "\n";
	info << "size:\n";
	info << " x: " << this->size.x << "\n";
	info << " y: " << this->size.y << "\n";
	info << " z: " << this->size.z << "\n";
	info.close();
}

void World::drawLights(){
	std::list<Light*>::iterator it;
	unsigned int currentLight = GL_LIGHT2;
	for (unsigned int i = GL_LIGHT2; i <= GL_LIGHT7; i++) {
		glDisable(i);
	}
	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	std::list <Light*> segonapas;
	for (it = lights.begin(); (it != lights.end() && currentLight <= GL_LIGHT7); it++) {
		if ((*it)->getEnabled()) {
			float dist = Vector3<float>::module((*it)->getPosVec() - camera->getPos());
			if ((camera->isVisible((*it)->getPosVec(), 0)) || (dist <= 1.0f)) { //Mostram nom�s les llums visibles o molt properes
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
	
	updateGeneration();
	//std::async(std::launch::async, &World::updateVisibility, this);//Provar un thread aposta? std::thread
	this->updateVisibility();
	//Ordenaci� llums
	lights.sort([this](Light* l1, Light* l2) {
		return (Vector3<float>::module(l1->getPosVec() - camera->getPos()) < Vector3<float>::module(l2->getPosVec() - camera->getPos()));
		}); //Ordenam les llums per dist�ncia

	/*updTimer -= delta;
	if (updTimer > 0) {
		return;

	}
	updTimer = 50;*/ //10 tps

	//NOU CODI CHUNKS
	pos = pos / CHUNKSIZE;
	pos.floor();
	int dist = floor(camera->getViewDist() / CHUNKSIZE / 3);
	for (int x = (int)(pos.x - dist); x < pos.x + dist; x++) {
		for (int y = (int)(pos.y - dist); y < pos.y + dist; y++) {
			for (int z = (int)(pos.z - dist); z < pos.z + dist; z++) {
				int desp = getDesp(Vector3<int>(x, y, z));
				if (desp != -1 && chunks[desp] != 0 && estat[desp] == ChunkState::LLEST) {
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

void World::updateGeneration() {
	if (pendents > 0) {
		for (int i = 0; i < genCores && pendents>0; i++) {
			if (cnk[i].valid()) {
				if (cnk[i].wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready) {
					Chunk* ch = cnk[i].get();
					Vector3 cpos = ch->getPos();
					int desp = getDesp(cpos);
					if (ch != nullptr) {
						pendents--;
						if (estat[desp] == ChunkState::PENDENT) {
							estat[desp] = ChunkState::TERRENY;
						}
						else if (estat[desp] == ChunkState::TERRENY) {
							chunks[desp]->updateMesh();
							estat[desp] = ChunkState::LLEST;
						}
						else if (estat[desp] == ChunkState::CARREGAT) {
							ch->updateMesh();
							estat[desp] = ChunkState::LLEST;
						}
						if (ch->nblocs <= 0) {
							estat[desp] = ChunkState::LLEST;
							delete ch;
							continue;
						}
						chunks[desp] = ch;
						if (estat[desp] == ChunkState::LLEST) {
							for (int nX = cpos.x - 1; nX <= cpos.x + 1; nX++) {
								for (int nY = cpos.y - 1; nY <= cpos.y + 1; nY++) {
									for (int nZ = cpos.z - 1; nZ <= cpos.z + 1; nZ++) {
										int desp = getDesp(Vector3<int>((int)nX, (int)nY, (int)nZ));
										if (desp == -1 || chunks[desp] == nullptr || estat[desp] != ChunkState::LLEST) {
											continue;
										}
										chunks[desp]->updateMesh();
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if (pendents < genCores) {
		std::list<Vector3<int>>::iterator chunki;
		for (chunki = vChunks.begin(); (chunki != vChunks.end()); chunki++) {
			Vector3<int> cPos = *chunki;
			int desp = getDesp(cPos);
			if (chunks[desp] == nullptr) {
				if (estat[desp] == ChunkState::BUIT && pendents < genCores) {
					bool trobat = false;
					for (int i = 0; (i < genCores) && !trobat; i++) {
						if (!cnk[i].valid()) {
							cnk[i] = std::async(std::launch::async, &WorldGenerator::generateTerrain, &wGen, cPos);
							estat[desp] = ChunkState::PENDENT;
							trobat = true;
							pendents++;
						}
					}
				}
				continue;
			}
			else if (estat[desp] == ChunkState::TERRENY && pendents < genCores) {
				bool trobat = false;
				bool possible = true;
				Vector3<int> nPos;
				for (nPos.x = cPos.x - 1; nPos.x <= cPos.x + 1 && possible; nPos.x++) {
					for (nPos.y = cPos.y - 1; nPos.y <= cPos.y + 1 && possible; nPos.y++) {
						for (nPos.z = cPos.z - 1; nPos.z <= cPos.z + 1 && possible; nPos.z++) {
							int desp = getDesp(nPos);
							if (estat[desp] != ChunkState::LLEST && estat[desp] != ChunkState::TERRENY) {
								possible = false;
							}
						}
					}
				}
				if (possible) {
					for (int i = 0; (i < genCores) && !trobat; i++) {
						if (!cnk[i].valid()) {
							trobat = true;
							cnk[i] = std::async(std::launch::async, &WorldGenerator::generateDetail, &wGen, chunks[desp]);
							pendents++;
						}
					}
				}
			}
		}
	}
}

//Alliberam la mem�ria de tots els blocs i entitats
void World::destroy() {

	//NOU CODI CHUNKS
	for (int i = 0; i < this->size.x * this->size.y * this->size.z; i++) {
		delete this->chunks[i];
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
	if (desp == -1 || chunks[desp] == nullptr) {
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

	if (chunks[desp] == nullptr){
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

	chunks[desp]->setBlock(bloc, bpos);

	if (listUpdate) {
		//chunks[desp]->updateDL();
		chunks[desp]->updateMesh();
		updateNeighborChunks(cpos, bpos);
	}
	return true;
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

	std::list<Entity*>::iterator ent;
	for (ent = entities.begin(); (ent != entities.end()); ent++) {
		Vector3<float> pos = (*ent)->getPos();
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		(*ent)->draw();
		glPopMatrix();
	}


	std::list<Vector3<int>>::iterator chunki;

	int nchunk = 0;
	for (chunki = vChunks.begin(); (chunki != vChunks.end()); chunki++) {
		Vector3<int> cPos = *chunki;
		int desp = getDesp(cPos);
		if (desp == -1 || chunks[desp] == NULL || estat[desp] != ChunkState::LLEST) {
			continue;
		}
		glPushMatrix();
		glTranslatef(cPos.x * CHUNKSIZE, cPos.y * CHUNKSIZE, cPos.z * CHUNKSIZE);
		chunks[desp]->drawO();
		glPopMatrix();
		if (nchunk < 5) {
			chunks[desp]->updateTransparency(camera->getPos());
		}
		nchunk++;
	}

	std::list<Vector3<int>>::reverse_iterator rchunki;
	for (rchunki = vChunks.rbegin(); (rchunki != vChunks.rend()); ++rchunki) {
		Vector3<int> cPos = *rchunki;
		int desp = getDesp(cPos);
		if (desp == -1 || chunks[desp] == NULL) {
			continue;
		}
		glPushMatrix();
		glTranslatef(cPos.x * CHUNKSIZE, cPos.y * CHUNKSIZE, cPos.z * CHUNKSIZE);
		chunks[desp]->drawT();
		glPopMatrix();
	}



	//for (chunki = vChunks.begin(); (chunki != vChunks.end()); chunki++) { //Transpar�ncia 2a passada
	//	Vector3<int> cPos = *chunki;
	//	int desp = getDesp(cPos);
	//	if (desp == -1 || chunks[desp] == NULL) {
	//		continue;
	//	}
	//	glPushMatrix();
	//	glTranslatef(cPos.x * CHUNKSIZE, cPos.y * CHUNKSIZE, cPos.z * CHUNKSIZE);
	//	chunks[desp]->drawT();
	//	glPopMatrix();
	//}

	//printf("Chunks: %d\n", nchunks);

	this->drawLights();
}

void World::updateVisibility() {

	//Obtenim el volum de possible visibilitat de la c�mera
	int xmin = camera->xmin / CHUNKSIZE;	int xmax = camera->xmax / CHUNKSIZE;
	int ymin = camera->ymin / CHUNKSIZE;	int ymax = camera->ymax / CHUNKSIZE;
	int zmin = camera->zmin / CHUNKSIZE;	int zmax = camera->zmax / CHUNKSIZE;

	xmin = std::max(xmin, 0);				ymin = std::max(ymin, 0);				zmin = std::max(zmin, 0);
	xmax = std::min(xmax, this->size.x);		ymax = std::min(ymax, this->size.y);		zmax = std::min(zmax, this->size.z);

	//NOU CODI CHUNKS
	Vector3<int> cMin = Vector3<int>(xmin, ymin, zmin);

	Vector3<int> cMax = Vector3<int>(xmax, ymax, zmax);

	vChunks.clear();
	//printf("%d %d %d, %d %d %d\n", cMin.x, cMin.y, cMin.z, cMax.x, cMax.y, cMax.z);
	int chunk = 0;
	Vector3<int> cPos = Vector3<int>(0, 0, 0);
	for (cPos.x = cMin.x; cPos.x <= cMax.x; cPos.x++) { //Optimitzaci� possible: si els chunks circumdants no son visibles, aquest no ho �s
		for (cPos.z = cMin.z; cPos.z <= cMax.z; cPos.z++) {
			for (cPos.y = cMin.y; cPos.y <= cMax.y; cPos.y++) {
				Vector3<float> pos = Vector3<float>(cPos.x, cPos.y, cPos.z) * CHUNKSIZE;
				float dist = Vector3<float>::module(camera->getPos() - (pos + Vector3<float>(CHUNKSIZE/2,CHUNKSIZE/2, CHUNKSIZE/2)));
				if ((dist < CHUNKSIZE * 4) || (camera->isVisible(pos, 100) ||
						camera->isVisible(pos + Vector3<float>(CHUNKSIZE, 0, 0), 100) ||
						camera->isVisible(pos + Vector3<float>(0, 0, CHUNKSIZE), 100) ||
						camera->isVisible(pos + Vector3<float>(CHUNKSIZE, 0, CHUNKSIZE), 100) ||
						camera->isVisible(pos + Vector3<float>(0, CHUNKSIZE, 0), 100) ||
						camera->isVisible(pos + Vector3<float>(CHUNKSIZE, CHUNKSIZE, 0), 100) ||
						camera->isVisible(pos + Vector3<float>(0, CHUNKSIZE, CHUNKSIZE), 100) ||
						camera->isVisible(pos + Vector3<float>(CHUNKSIZE, CHUNKSIZE, CHUNKSIZE), 100))) {
					vChunks.push_front(cPos);
					chunk++;
				}
			}
		}
	}
	vChunks.sort([this](Vector3<int> a, Vector3<int> b) {
		Vector3<int> half = Vector3<int>(CHUNKSIZE / 2, CHUNKSIZE / 2, CHUNKSIZE / 2);
		return (Vector3<int>::module((a*CHUNKSIZE + half) - camera->getPos()) < Vector3<int>::module((b * CHUNKSIZE + half) - camera->getPos())); }
	);
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
	int desp = pos.x + this->size.x * (pos.y + this->size.y * pos.z);
	if ((desp >= (this->size.x * this->size.y * this->size.z))||(desp < 0) || pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= this->size.x || pos.y >= this->size.y || pos.z >= this->size.z) {
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


//TODO: que faci una llista amb els chunks a actualitzar
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
		//chunks[desp]->firstdraw = true;
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
		//chunks[desp]->firstdraw = true;
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
		//chunks[desp]->firstdraw = true;
	}
}

Vector3<int> World::getSpawn() {
	return this->spawn;
}

void World::drawMap(float scrAspect, Entity *ent, int y) {
	glPushMatrix();
	float aspect = scrAspect;
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glColor3f(1, 1, 1);
	glBegin(GL_QUADS); //Quadrat blanc exterior
	glVertex3f(0.5f * aspect - 0.4f, 0.9f, -1);
	glVertex3f(0.5f * aspect - 0.4f, 0.1f, -1);
	glVertex3f(0.5f * aspect + 0.4f, 0.1f, -1);
	glVertex3f(0.5f * aspect + 0.4f, 0.9f, -1);
	glEnd();

	glColor4f(0, 0, 0, 1);
	glLineWidth(3.0f);
	glBegin(GL_LINES); //L�nies que indiquen els l�mits del dibuixat del mapa
	glVertex3f(0.5f * aspect - 0.4f, 0.9f, -1);
	glVertex3f(0.5f * aspect - 0.4f, 0.1f, -1);

	glVertex3f(0.5f * aspect + 0.4f, 0.1f, -1);
	glVertex3f(0.5f * aspect + 0.4f, 0.9f, -1);

	glVertex3f(0.5f * aspect - 0.4f, 0.1f, -1);
	glVertex3f(0.5f * aspect + 0.4f, 0.1f, -1);

	glVertex3f(0.5f * aspect - 0.4f, 0.9f, -1);
	glVertex3f(0.5f * aspect + 0.4f, 0.9f, -1);
	glEnd();

	float dX = 0.8f * ((float)1 / (float)this->size.x);
	float dZ = 0.8f * ((float)1 / (float)this->size.z);

	glTranslatef(0.5f * aspect - 0.4f, 0.1f, -1);
	for (int x = 0; x < this->size.x; x++) {
		for (int z = 0; z < this->size.z; z++) {
			glPushMatrix();
			int desp = getDesp(Vector3<int>(x, y, z));
			glTranslatef(((float)x / (float)this->size.x)*0.8f, ((float)z / (float)this->size.z)*0.8f, 0);
			if (chunks[desp] == nullptr || desp == -1) {
				glColor3f(0,0,0);
			}
			else {
				switch (chunks[desp]->getBiome()) {
					case Bioma::MUNTANYA:
						glColor3f(0.8f,0.8f,0.8f);
						break;
					case Bioma::BOSC:
						glColor3f(1, 0, 0);
						break;
					case Bioma::OCEA:
						glColor3f(0, 0, 0.5f);
						break;
					case Bioma::MAR:
						glColor3f(0, 0, 1);
						break;
					case Bioma::PLANA:
						glColor3f(0, 1, 0);
						break;
					case Bioma::DESERT:
						glColor3f(0.76f, 0.60f, 0.42f);
						break;
					case Bioma::SABANA:
						glColor3f(1, 1, 0);
						break;
					case Bioma::ARTIC:
						glColor3f(1, 1, 1);
						break;
					case Bioma::GEL:
						glColor3f(0.5f, 0.5f, 1);
						break;
					default:
						glColor3f(1, 0, 0);
						break;
				}
			}
			glBegin(GL_QUADS);
			glVertex2f(0, dZ);
			glVertex2f(dX, dZ);
			glVertex2f(dX, 0);
			glVertex2f(0, 0);
			glEnd();
			glPopMatrix();
		}
	}
	glPushMatrix();

	//Dibuix del jugador

	float pX = ent->getPos().x / CHUNKSIZE;
	float pZ = ent->getPos().z / CHUNKSIZE;
	glTranslatef((pX / (float)this->size.x) * 0.8f + dX/2, (pZ / (float)this->size.z) * 0.8f + dZ/2, 0);
	glRotatef(ent->getRot() - 90, 0, 0, 1);
	glColor3f(1, 1, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(-dX/2, -dZ/2);
	glVertex2f(0, dZ/2);
	glVertex2f(dX/2, -dZ/2);
	glEnd();
	//Petit indica visi� jugador
	glColor3f(0, 0, 1);
	glBegin(GL_TRIANGLES);
	glVertex2f(dX /4, 0);
	glVertex2f(0, dZ / 2);
	glVertex2f(-dX / 4, 0);
	glEnd();
	glPopMatrix();

	glPopMatrix();
}

Vector3<int> World::getRegion(Vector3<int> cPos) {
	return Vector3<int>(floor((float)cPos.x / (float)REGIONSIZE), floor((float)cPos.y / (float)REGIONSIZE), floor((float)cPos.z / (float)REGIONSIZE));
}

bool World::saveRegion(Vector3<int> rPos) {
	const int chunkSize = CHUNKSIZE * CHUNKSIZE * CHUNKSIZE;
	char buffer[chunkSize];
	std::fstream file; 
	std::string path = "worlds/" + this->name + "/chunks/reg" + std::to_string(rPos.x) + "-" + std::to_string(rPos.y) + "-" + std::to_string(rPos.z) + ".cnk";
	printf("%s\n", path.c_str());
	file.open(path.c_str(), std::ios::out | std::ios::binary);
	for (int x = rPos.x * REGIONSIZE; x < (rPos.x+1) * REGIONSIZE; x++) {
		for (int y = rPos.y * REGIONSIZE; y < (rPos.y + 1) * REGIONSIZE; y++) {
			for (int z = rPos.z * REGIONSIZE; z < (rPos.z + 1) * REGIONSIZE; z++) {
				Vector3<int> rPos = getRegion(Vector3<int>(x, y, z));
				int desp = getDesp(Vector3<int>(x, y, z));
				if (chunks[desp] != nullptr && desp != -1) {
					buffer[0] = static_cast<int>(chunks[desp]->getBiome());
					file.write(buffer, 1);
					chunks[desp]->getByteData(buffer);
					file.write(buffer, chunkSize);
				}
				else {
					char zeros[chunkSize + 1];
					memset(zeros, 0, chunkSize + 1); //+1 per el byte de bioma
					file.write(zeros, chunkSize + 1);
				}
			}
		}
	}
	file.close();
	return true;
}

bool World::loadRegion(Vector3<int> rPos) {
	const int chunkSize = CHUNKSIZE * CHUNKSIZE * CHUNKSIZE;
	char buffer[chunkSize];
	char zeros[chunkSize];
	memset(zeros, 0, chunkSize);
	std::fstream file;
	std::string path = "worlds/" + this->name + "/chunks/reg" + std::to_string(rPos.x) + "-" + std::to_string(rPos.y) + "-" + std::to_string(rPos.z) + ".cnk";
	printf("%s\n", path.c_str());
	file.open(path.c_str(), std::ios::in | std::ios::binary);
	for (int x = rPos.x * REGIONSIZE; x < (rPos.x + 1) * REGIONSIZE; x++) {
		for (int y = rPos.y * REGIONSIZE; y < (rPos.y + 1) * REGIONSIZE; y++) {
			for (int z = rPos.z * REGIONSIZE; z < (rPos.z + 1) * REGIONSIZE; z++) {
				int desp = getDesp(Vector3<int>(x, y, z));
				file.read(buffer, 1);
				Bioma bio = static_cast<Bioma>(buffer[0]);
				file.read(buffer, chunkSize);
				if (memcmp(buffer, zeros, chunkSize) != 0) { //Si el chunk no �s buit, el carregam
					Vector3<int> cpos = Vector3<int>(x, y, z);
					chunks[desp] = new Chunk(this, cpos);
					chunks[desp]->readFromByteData(buffer);
					chunks[desp]->setBiome(bio);
					chunks[desp]->updateMesh();
					for (int nX = cpos.x - 1; nX <= cpos.x + 1; nX++) {
						for (int nY = cpos.y - 1; nY <= cpos.y + 1; nY++) {
							for (int nZ = cpos.z - 1; nZ <= cpos.z + 1; nZ++) {
								int desp = getDesp(Vector3<int>((int)nX, (int)nY, (int)nZ));
								if (desp == -1 || chunks[desp] == nullptr || estat[desp] != ChunkState::LLEST) {
									continue;
								}
								chunks[desp]->updateMesh();
							}
						}
					}
					estat[desp] = ChunkState::LLEST;
				}
			}
		}
	}
	file.close();
	return true;
}