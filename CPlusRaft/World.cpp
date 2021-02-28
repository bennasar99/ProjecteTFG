#include "World.h"

World::World(std::string name, int seed, int sizex, int sizey, int sizez, Camera* camera) //Nou
{

	this->genCores = ThreadManager::getCoreCount() - 2;
	this->genCores = std::max(this->genCores, 1);

	this->cnk = std::vector< std::future<Chunk*> >(genCores);
	this->name = name;
	this->wGen = WorldGenerator(seed, this);

	this->seed = seed;
	this->camera = camera;

	//Col·locam càmera
	int x = rand() % (512 * CHUNKSIZE);
	int z = rand() % (512 * CHUNKSIZE);
	bool trobat = false;
	for (int y = 256; y > 0 && (!trobat); y--) {
		Bloc b = getBlock(Vector3<int>(x, y, z));
		if (b != Bloc::RES) {
			trobat = true;
			spawn = Vector3<int>(x, y + 2, z);
		}
	}
}

World::World(std::string name, Camera* camera) { //Càrrega ja existent

	this->genCores = ThreadManager::getCoreCount() - 2;
	this->genCores = std::max(this->genCores, 1);

	this->seed = 0;
	this->name = name;

	this->camera = camera;
	this->minpos = Vector3<int>(INT_MAX, INT_MAX, INT_MAX);

	//Lectura informació món
	std::ifstream info("worlds/" + name + "/info.yml");
	std::string str((std::istreambuf_iterator<char>(info)), std::istreambuf_iterator<char>());
	char *par = (char*)str.c_str();
	c4::substr parse = c4::to_substr(par);
	ryml::Tree tree = ryml::parse(parse);
	ryml::read(tree["seed"], &this->seed);
	ryml::read(tree["spawn"]["x"], &this->spawn.x);
	ryml::read(tree["spawn"]["y"], &this->spawn.y);
	ryml::read(tree["spawn"]["z"], &this->spawn.z);
	printf("Spawn del món a: %d %d %d. Seed %d\n", this->spawn.x, this->spawn.y, this->spawn.z, this->seed);
	info.close();

	std::fstream file;
	this->cnk = std::vector< std::future<Chunk*> >(genCores);
	this->wGen = WorldGenerator(this->seed, this);

	//Carregam les regions
	string path = "worlds/" + name + "/chunks/";

	for (const auto& file : std::filesystem::directory_iterator(path))
		loadFile(file.path().string());

	/*for (int rX = ceil((float)this->spawn.x - SPAWNSIZE/2)/REGIONSIZE; rX < ceil((float)this->spawn.x + SPAWNSIZE / 2)/REGIONSIZE; rX++) {
		for (int rY = ceil((float)this->spawn.y - SPAWNSIZE / 2)/REGIONSIZE; rY < ceil((float)this->spawn.y + SPAWNSIZE / 2)/REGIONSIZE; rY++) {
			for (int rZ = ceil((float)this->spawn.z - SPAWNSIZE / 2)/REGIONSIZE; rZ < ceil((float)this->spawn.z + SPAWNSIZE / 2)/REGIONSIZE; rZ++) {
				loadRegion(Vector3<int>(rX, rY, rZ));
			}
		}
	}*/

	//printf("Mides del mon: %d %d %d, spawn a: %d %d %d. Seed %d\n", this->size.x, this->size.y, this->size.z, sX, sY, sZ, this->seed);
}

void World::save() {
	
	std::string path = "worlds/" + this->name;
	std::filesystem::create_directory(path.c_str());
	path += "/chunks";
	std::filesystem::create_directory(path.c_str());

	std::list<Vector3<int>> regions;
	std::map<Vector3<int>, Chunk*>::iterator cit;
	for (cit = chunks.begin(); cit != chunks.end(); cit++) {
		if (cit->second != nullptr && estat[cit->first] == ChunkState::LLEST && cit->second->getDirty()) {
			regions.push_front(getRegion(cit->first));
		}
	}

	struct comparator {
		bool operator() (Vector3<int> v1, Vector3<int> v2)
		{
			return (v1 == v2);
		}
	};
	regions.sort([](Vector3<int>& v1, Vector3<int>& v2) {
		return make_tuple(v1.x, v1.y, v1.z) < make_tuple(v2.x, v2.y, v2.z);
		});
	regions.unique(comparator());

	std::list<Vector3<int>>::iterator it;
	for (it = regions.begin(); it != regions.end(); it++) {
		saveRegion(*it);
	}

	//Cream yml del món
	std::ofstream info("worlds/" + name + "/info.yml");
	info << "seed: " << seed << "\n";
	info << "spawn:\n";
	info << " x: " << this->camera->getPos().x << "\n";
	info << " y: " << this->camera->getPos().y << "\n";
	info << " z: " << this->camera->getPos().z << "\n";
	info.close();
}

void World::drawLights(){
	glPushMatrix();
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
			if ((camera->isVisible((*it)->getPosVec(), 0)) || (dist <= 1.0f)) { //Mostram només les llums visibles o molt properes
				setLight(currentLight, *it);
				currentLight++;
			}
			else {
				segonapas.push_back(*it); //Ho posam darrera per després ja tenir-ho ordenat per distància
			}
		}
	}

	//2a passada, només tenim en compte la distància
	for (it = segonapas.begin(); (it != segonapas.end() && currentLight <= GL_LIGHT7); it++) {
		setLight(currentLight, *it);
		currentLight++;
	}
	//glPopMatrix();

	//Sol, establim la seva posició segons el moment del dia
	this->solpos = Vector3<float>(cosf((daytime / (DAYTIME/2)) * 3.1416f), sinf((daytime / (DAYTIME/2)) * 3.1416f), 0.0f);
	GLfloat spos[4] = { solpos.x, solpos.y, solpos.z, 0.0f };
	glPopMatrix();

	//Establim llum ambient segons el moment del dia.
	glLightfv(sol, GL_POSITION, spos);
	float ambient = 0.3f + sinf((daytime / (DAYTIME / 2))* M_PI) * 2.7f;
	ambient = std::max(0.3f, ambient);
	GLfloat lluma[4] = { ambient, ambient, ambient, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lluma); //Posar a 0.1
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

//Afegeix un llum al món
Light* World::addLight(Vector3<float> pos) {
	Light* llum = new Light(pos);
	this->lights.push_front(llum);
	return llum;
}

//Elimina una llum del món
void World::delLight(Light* light) {
	this->lights.remove(light);
	delete light;
}

/**
 * Actualització del món a cada frame (Idle)
 * @param delta temps des de la darrera actualització en ms
 * @param Vector3<float> posicio
 * Actualitza l'estat intern de les estructures del món, a més de la distància de les llums
 */
void World::update(float delta, Vector3<float> pos) {

	daytime += ((float)delta);
	if (daytime >= DAYTIME) {
		daytime = 0; //Reiniciam el temps del dia
		glEnable(sol); //Tornam activar el sol
	}
	else if (daytime > (DAYTIME/2)) { //Si es de vespre, desactivam el sol
		glDisable(sol);
	}
	
	updateGeneration();
	//std::async(std::launch::async, &World::updateVisibility, this);//Provar un thread aposta? std::thread
	this->updateVisibility();
	//Ordenació llums
	lights.sort([this](Light* l1, Light* l2) {
		return (Vector3<float>::module(l1->getPosVec() - camera->getPos()) < Vector3<float>::module(l2->getPosVec() - camera->getPos()));
		}); //Ordenam les llums per distància

	/*updTimer -= delta;
	if (updTimer > 0) {
		return;

	}
	updTimer = 50;*/ //10 tps

	//NOU CODI CHUNKS
	Vector3<int> pcPos = getChunkPos(pos.toInt());

	//Descàrrega de chunks
	Vector3<int> prPos = getRegion(pcPos); //Obtenim la regió actual del jugador
	int range = ceil(ceil(camera->getViewDist() / CHUNKSIZE) / REGIONSIZE);  //"Distància de visió en regions"

	std::map<Vector3<int>, bool, Vector3Compare> toSave;
	std::vector<Chunk*> toDelete;

	std::map<Vector3<int>, Chunk*>::iterator cit;
	for (cit = chunks.begin(); cit != chunks.end(); cit++) {
		Chunk* ch = cit->second;
		if (ch == nullptr) {
			continue;
		}
		Vector3<int> cPos = cit->first;
		Vector3<int> rPos = getRegion(cPos);
		float dist = Vector3<int>::module(prPos - rPos);
		if (dist <= range) { //Si està dins el rang de regions properes, no feim res
			continue;
		}
		bool dirty = ch->getDirty();
		if (dirty && estat[cit->first] == ChunkState::LLEST) { //S'ha de guardar el chunk abans de descarregar-lo de memòria
			toSave[rPos] = true;
		}
		toDelete.push_back(ch);
	}

	std::map<Vector3<int>, bool>::iterator it2;
	for (it2 = toSave.begin(); it2 != toSave.end(); it2++) {
		if (it2->second == false) {
			continue;
		}
		saveRegion(it2->first);
	}

	std::vector<Chunk*>::iterator it3;
	for (it3 = toDelete.begin(); it3 != toDelete.end(); it3++) {
		Vector3<int> cPos = (*it3)->getPos();
		chunks.erase(cPos);
		estat.erase(cPos);
		Chunk* ch = *it3;
		delete ch;
	}

	int dist = floor(camera->getViewDist() / CHUNKSIZE / 3);
	for (int x = pcPos.x - dist; x < pcPos.x + dist; x++) {
		for (int y = pcPos.y - dist; y < pcPos.y + dist; y++) {
			for (int z = pcPos.z - dist; z < pcPos.z + dist; z++) {
				Vector3<int> cPos = Vector3<int>(x, y, z);
				Chunk* ch = chunks[cPos];
				if (ch != 0 && estat[cPos] == ChunkState::LLEST) {
					ch->update(delta);
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
					Vector3 cPos = ch->getPos();
					if (ch != nullptr) {
						pendents--;
						if (estat[cPos] == ChunkState::PENDENT) {
							estat[cPos] = ChunkState::TERRENY;
						}
						else if (estat[cPos] == ChunkState::TERRENY) {
							ch->updateMesh();
							estat[cPos] = ChunkState::LLEST;
							ch->setDirty(false);
						}
						else if (estat[cPos] == ChunkState::CARREGAT) {
							ch->updateMesh();
							estat[cPos] = ChunkState::LLEST;
							ch->setDirty(false);
						}
						/*if (ch->nblocs <= 0) { //Amb marching cubes no es pot fer ja que un chunk pot haver de dibuixar triangles sense tenir blocs
							estat[cPos] = ChunkState::LLEST;
							delete ch;
							continue;
						}*/
						chunks[cPos] = ch;
						if (estat[cPos] == ChunkState::LLEST && ch->nblocs > 0) {
							updateNeighborChunks(cPos);
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
			Chunk* ch = chunks[cPos];
			if (ch == nullptr) {
				if (estat[cPos] == ChunkState::BUIT && pendents < genCores) {
					bool trobat = false;
					for (int i = 0; (i < genCores) && !trobat; i++) {
						if (!cnk[i].valid()) {
							cnk[i] = std::async(std::launch::async, &WorldGenerator::generateTerrain, &wGen, cPos);
							estat[cPos] = ChunkState::PENDENT;
							trobat = true;
							pendents++;
						}
					}
				}
				continue;
			}
			else if (estat[cPos] == ChunkState::TERRENY && pendents < genCores) {
				bool trobat = false;
				bool possible = true;

				//IDEA: llista de canvis pendents per cada Chunk, s'aplicaran en acabar la generació, així aquest control podria no fer falta
				Vector3<int> nPos;
				for (nPos.x = cPos.x - 1; nPos.x <= cPos.x + 1 && possible; nPos.x++) {
					for (nPos.y = cPos.y - 1; nPos.y <= cPos.y + 1 && possible; nPos.y++) {
						for (nPos.z = cPos.z - 1; nPos.z <= cPos.z + 1 && possible; nPos.z++) {
							if (estat[nPos] != ChunkState::LLEST && estat[nPos] != ChunkState::TERRENY) {
								possible = false;
							}
						}
					}
				}
				if (possible) {
					for (int i = 0; (i < genCores) && !trobat; i++) {
						if (!cnk[i].valid()) {
							trobat = true;
							cnk[i] = std::async(std::launch::async, &WorldGenerator::generateDetail, &wGen, ch);
							pendents++;
						}
					}
				}
			}
		}
	}
}

//Alliberam la memòria de tots els blocs i entitats
void World::destroy() {
	std::map<Vector3<int>, Chunk*>::iterator cit;
	for (cit = chunks.begin(); cit != chunks.end(); cit++) {
		delete cit->second;
	}

	std::list<Entity*>::iterator ent;
	for (ent = entities.begin(); (ent != entities.end()); ent++) {
		(*ent)->destroy();
		delete *ent;
	}
}

//Assignam un llum (GL_LIGHT0-7) al sol
//void World::setSol(int sol) {
//	this->sol = sol;
//	GLfloat soldiff[4] = { 1, 1, 1, 1.0f };
//	glLightfv(sol, GL_DIFFUSE, soldiff);
//	glLightf(sol, GL_SPOT_CUTOFF, 180.0f);
//	glLightf(sol, GL_SPOT_EXPONENT, 0.0f);
//	glEnable(sol); //pensa a tornar posar Enable
//}

//Eliminam el bloc de la posició indicada
bool World::deleteBlock(Vector3<int> pos, bool destroy) { //Eliminar Bloc::RES?

	//pos.floor();
	Vector3<int> cPos = getChunkPos(pos);
	//cpos.floor();
	Vector3<int> bpos = pos % CHUNKSIZE;
	Chunk* ch = chunks[cPos];
	if (ch == nullptr) {
		return false;
	}
	return ch->delBlock(bpos, destroy);
}

//Col·locam un bloc d'un tipus determinat a la posició indicada
bool World::setBlock(Bloc tipus, Vector3<int> pos, bool overwrite, bool listUpdate) {

	//TONI!! -16 ha de ser des CHunk -1, no -2!!!!!

	//NOU CODI CHUNKS:
	//pos.floor();
	Vector3<int> cPos = getChunkPos(pos);

	//cpos.floor();
	Vector3<int> bpos = pos % CHUNKSIZE;

	if (listUpdate) {
		printf("set block on chunk %d %d %d\n", cPos.x, cPos.y, cPos.z);
		printf("set block on pos %d %d %d\n", bpos.x, bpos.y, bpos.z);
	}

	if (chunks[cPos] == nullptr){
		chunks[cPos] = new Chunk(this, cPos);
	}
	Chunk* ch = chunks[cPos];

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
	ch->setBlock(bloc, bpos);
	if (listUpdate) {
		ch->updateMesh();
		updateNeighborChunks(cPos, bpos);
	}

	//Actualitzam maxpos i minpos
	maxpos.x = std::max(maxpos.x, pos.x); minpos.x = std::min(minpos.x, pos.x);
	maxpos.y = std::max(maxpos.y, pos.y); minpos.y = std::min(minpos.y, pos.y);
	maxpos.z = std::max(maxpos.z, pos.z); minpos.z = std::min(minpos.z, pos.z);

	return true;
}

//Assignam un bloc (per punter) a la posició indicada
bool World::setBlock(Block* bloc, Vector3<int> pos, bool listUpdate) {
	
	Vector3<int> cPos = getChunkPos(pos);
	//cpos.floor();
	Vector3<int> bpos = pos % CHUNKSIZE;
	Chunk* ch = chunks[cPos];

	//Actualitzam maxpos i minpos
	maxpos.x = std::max(maxpos.x, cPos.x); minpos.x = std::min(minpos.x, cPos.x);
	maxpos.y = std::max(maxpos.y, cPos.y); minpos.y = std::min(minpos.y, cPos.y);
	maxpos.z = std::max(maxpos.z, cPos.z); minpos.z = std::min(minpos.z, cPos.z);

	ch->setBlock(bloc, bpos);

	if (listUpdate) {
		ch->updateMesh();
		updateNeighborChunks(cPos, bpos);
	}
	return true;
}

//Obtenim el bloc a la posició indicada
Bloc World::getBlock(Vector3<float> pos) {
	pos.floor();
	return getBlock(pos.toInt());
}

Bloc World::getBlock(Vector3<int> pos) {
	Vector3<int> cPos = getChunkPos(pos);
	//cpos.floor();
	Vector3<int> bpos = pos % CHUNKSIZE;
	Chunk* ch = chunks[cPos];
	if (ch == nullptr || estat[cPos] != ChunkState::LLEST) {
		return Bloc::RES;
	}
	return ch->getBlock(bpos);
}

//remove indica si el bloc s'ha d'eliminar del món (NO S'ALLIBERA LA MEMÒRIA)
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


//Dibuixa el món visible
void World::draw(Vector3<float> pos, float dist) {

	std::list<Entity*>::iterator ent;
	for (ent = entities.begin(); (ent != entities.end()); ent++) {
		Vector3<float> pos = (*ent)->getPos();
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		(*ent)->draw();
		glPopMatrix();
	}

	std::list<Vector3<int>> vChunks;
	{
		const std::lock_guard<std::mutex> lock(mutex);
		vChunks = std::list<Vector3<int>>(this->vChunks);
	}

	std::list<Vector3<int>>::iterator chunki;

	int nchunk = 0;
	for (chunki = vChunks.begin(); (chunki != vChunks.end()); chunki++) {
		Vector3<int> cPos = *chunki;
		Chunk* ch = chunks[cPos];
		if (ch == NULL || estat[cPos] != ChunkState::LLEST) {
			continue;
		}
		glPushMatrix();
		glTranslatef(cPos.x * CHUNKSIZE, cPos.y * CHUNKSIZE, cPos.z * CHUNKSIZE);
		ch->drawO();
		glPopMatrix();
		if (nchunk < 5) {
			ch->updateTransparency(camera->getPos());
		}
		nchunk++;
	}

	std::list<Vector3<int>>::reverse_iterator rchunki;
	for (rchunki = vChunks.rbegin(); (rchunki != vChunks.rend()); ++rchunki) {
		Vector3<int> cPos = *rchunki;
		Chunk *ch = chunks[cPos];
		if (ch == NULL || estat[cPos] != ChunkState::LLEST) {
			continue;
		}
		glPushMatrix();
		glTranslatef(cPos.x * CHUNKSIZE, cPos.y * CHUNKSIZE, cPos.z * CHUNKSIZE);
		ch->drawT();
		glPopMatrix();
	}

	//printf("Chunks: %d\n", nchunks);

	this->drawLights();
}

void World::updateVisibility() {

	//Obtenim el volum de possible visibilitat de la càmera
	Vector3<int> cMin = getChunkPos(Vector3<int>(camera->xmin, camera->ymin, camera->zmin));
	Vector3<int> cMax = getChunkPos(Vector3<int>(camera->xmax, camera->ymax, camera->zmax)) + Vector3(1,1,1);

	std::list<Vector3<int>> vChunks;
	//printf("%d %d %d, %d %d %d\n", cMin.x, cMin.y, cMin.z, cMax.x, cMax.y, cMax.z);
	int chunk = 0;
	Vector3<int> cPos = Vector3<int>(0, 0, 0);
	for (cPos.x = cMin.x; cPos.x <= cMax.x; cPos.x++) { //Optimització possible: si els chunks circumdants no son visibles, aquest no ho és
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
	{
		const std::lock_guard<std::mutex> lock(mutex);
		this->vChunks.clear();
		this->vChunks = vChunks;
	}
}

//Dibuixa un bloc a una posició determinada (Sense guardar-lo al món)
void World::drawBloc(Bloc tipus) {
	Block::drawIcon(tipus);
}

//Dibuixa l'eix de coordenades a la posició indicada
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
	glPushMatrix();

	//El sol no s'ha de veure afectat per la boira o l'il·luminació
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glTranslatef(pos.x, pos.y, pos.z);
	glTranslatef(solpos.x * dist, solpos.y * dist, solpos.z);
	glColor3f(1, 1, 0);
	drawSphere(1, 5, 5);
	glEnable(GL_LIGHTING);
	glEnable(GL_FOG);
		
	//Establim el color del cel (+ blanc com + adalt sigui el sol)
	//Vespre: 	0, 0, 54.5, dia: 	52.9, 80.8, 92.2, alba/posta: 	100, 64.7, 0
	float aY = abs(sinf((daytime / (DAYTIME / 2)) * M_PI));
	float aX = abs(cosf((daytime / (DAYTIME / 2)) * M_PI));
	if (daytime > DAYTIME / 2) { //Vespre: 	0, 0, 54.5, alba/posta: 	100, 64.7, 0
		glClearColor(1*aX, 0.647*aX, 0.545f * aY, 1);
	}
	else { //dia: 	52.9, 80.8, 92.2, alba/posta: 	100, 64.7, 0
		glClearColor(0.529f + 0.471f * aX, 0.647f + 0.1538f * aY, 0.92 * aY, 1);
	}

	glPopMatrix();
}

//Interacció amb un bloc
void World::interact(Vector3<int> pos) {
	//pos.floor();
	Vector3<int> cPos = getChunkPos(pos);
	//cpos.floor();
	Vector3<int> bpos = Vector3<int>(pos.x % CHUNKSIZE, pos.y % CHUNKSIZE, pos.z % CHUNKSIZE);

	return chunks[cPos]->interact(bpos);
}

//Afegeix una entitat al món
Entity* World::addEntity(Entitat ent, Vector3<float> pos) {
	Entity* entitat = 0;
	//switch (ent) {
		//Ficar entitat amb new a *entitat
	//}
	entities.push_front(entitat);
	return entitat;
}

//Elimina una entitat del món
void World::delEntity(Entity* ent) {
	ent->destroy();
	entities.remove(ent); //L'eliminam de la llista
	delete ent; //Alliberam la seva memoria
}

//Obté l'entitat més propera a una posició dins la distància (range) indicada, podent requerir que sigui controlable.
Entity* World::getNearestEntity(Vector3<float> pos, float range, bool controllable){
	Entity* entitat = 0;
	std::list<Entity*>::iterator ent;
	float currDist = range;
	for (ent = entities.begin(); (ent != entities.end()); ent++) {
		float dist = Vector3<float>::module((*ent)->getPos() - pos);
		if (controllable && !(*ent)->getControllable()) { //Si no és controlable i ho ha de ser, passam a la següent
			continue;
		}
		if (dist < currDist){
			entitat = (*ent);
			currDist = dist;
		}
	}
	return entitat;
}

void World::updateNeighborChunks(Vector3<int> cPos) {
	Vector3<int> oPos;
	for (oPos.x = cPos.x - 1; oPos.x <= (cPos.x + 1); oPos.x++) {
		for (oPos.y = cPos.y - 1; oPos.y <= (cPos.y + 1); oPos.y++) {
			for (oPos.z = cPos.z - 1; oPos.z <= (cPos.z + 1); oPos.z++) {
				if (oPos == cPos) {
					continue;
				}
				if (chunks[oPos] == nullptr || estat[oPos] != ChunkState::LLEST) {
					continue;
				}
				chunks[oPos]->updateMesh();
			}
		}
	}
}

void World::updateNeighborChunks(Vector3<int> cpos, Vector3<int> bpos) {
	std::list<Vector3<int>> toUpdate;
	if (bpos.x == CHUNKSIZE-1) {
		toUpdate.push_front(cpos + Vector3<int>(1, 0, 0));
	}
	else if (bpos.x == 0) {
		toUpdate.push_front(cpos - Vector3<int>(1, 0, 0));
	}
	if (bpos.y == CHUNKSIZE-1) {
		toUpdate.push_front(cpos + Vector3<int>(0, 1, 0));
	}
	else if (bpos.y == 0) {
		toUpdate.push_front(cpos - Vector3<int>(0, 1, 0));
	}
	if (bpos.z == CHUNKSIZE-1) {
		toUpdate.push_front(cpos + Vector3<int>(0, 0, 1));
	}
	else if (bpos.z == 0) {
		toUpdate.push_front(cpos - Vector3<int>(0, 0, 1));
	}
	std::list<Vector3<int>>::iterator chunki;
	for (chunki = toUpdate.begin(); (chunki != toUpdate.end()); chunki++) {
		if (estat[*chunki] != ChunkState::LLEST) {
			continue;
		}
		Chunk* ch = chunks[*chunki];
		if (ch == nullptr) {
			ch = new Chunk(this, *chunki);
			chunks[*chunki] = ch;
		}

		ch->updateMesh();
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
	glBegin(GL_LINES); //Línies que indiquen els límits del dibuixat del mapa
	glVertex3f(0.5f * aspect - 0.4f, 0.9f, -1);
	glVertex3f(0.5f * aspect - 0.4f, 0.1f, -1);

	glVertex3f(0.5f * aspect + 0.4f, 0.1f, -1);
	glVertex3f(0.5f * aspect + 0.4f, 0.9f, -1);

	glVertex3f(0.5f * aspect - 0.4f, 0.1f, -1);
	glVertex3f(0.5f * aspect + 0.4f, 0.1f, -1);

	glVertex3f(0.5f * aspect - 0.4f, 0.9f, -1);
	glVertex3f(0.5f * aspect + 0.4f, 0.9f, -1);
	glEnd();

	float dX = 0.8f * ((float)1 / (float)SPAWNSIZE);
	float dZ = 0.8f * ((float)1 / (float)SPAWNSIZE);

	Vector3<int> cPos = getChunkPos(ent->getPos().toInt());
	glTranslatef(0.5f * aspect - 0.4f, 0.1f, -1);
	float mX = 0;
	float mZ = 0;
	for (int x = cPos.x - SPAWNSIZE/2; x <= cPos.x + SPAWNSIZE/2; x++) {
		mZ = 0;
		for (int z = cPos.z - SPAWNSIZE/2; z <= cPos.z + SPAWNSIZE/2; z++) {
			Vector3 cPos = Vector3<int>(x, y, z);
			glPushMatrix();
			glTranslatef(mX * dX, mZ * dZ, 0);
			Chunk* ch = chunks[cPos];
			if (ch == nullptr) {
				glColor3f(0,0,0);
			}
			else {
				switch (ch->getBiome()) {
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
			mZ++;
		}
		mX++;
	}
	glPushMatrix();

	//Dibuix del jugador

	glTranslatef(0.4f, 0.4f, 0);
	glRotatef(ent->getRot() - 90, 0, 0, 1);
	glColor3f(1, 1, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(-dX/2, -dZ/2);
	glVertex2f(0, dZ/2);
	glVertex2f(dX/2, -dZ/2);
	glEnd();
	//Petit indica visió jugador
	glColor3f(0, 0, 1);
	glBegin(GL_TRIANGLES);
	glVertex2f(dX /4, 0);
	glVertex2f(0, dZ / 2);
	glVertex2f(-dX / 4, 0);
	glEnd();
	glPopMatrix();

	glPopMatrix();
}


bool World::saveRegion(Vector3<int> rPos) {
	const int chunkSize = CHUNKSIZE * CHUNKSIZE * CHUNKSIZE;
	char buffer[chunkSize];
	std::fstream file; 
	std::string path = "worlds/" + this->name + "/chunks/reg" + std::to_string(rPos.x) + "_" + std::to_string(rPos.y) + "_" + std::to_string(rPos.z) + ".cnk";
	printf("%s\n", path.c_str());
	file.open(path.c_str(), std::ios::out | std::ios::binary);
	int nChunks = 0;
	for (int x = rPos.x * REGIONSIZE; x < (rPos.x+1) * REGIONSIZE; x++) {
		for (int y = rPos.y * REGIONSIZE; y < (rPos.y + 1) * REGIONSIZE; y++) {
			for (int z = rPos.z * REGIONSIZE; z < (rPos.z + 1) * REGIONSIZE; z++) {
				Vector3<int> rPos = getRegion(Vector3<int>(x, y, z));
				Vector3 cPos = Vector3<int>(x, y, z);
				Chunk* ch = chunks[cPos];
				if (ch != nullptr && estat[cPos] == ChunkState::LLEST && ch->nblocs > 0 && ch->getDirty()) {
					buffer[0] = static_cast<int>(ch->getBiome());
					//Vector3<int> pos = cPos % REGIONSIZE;
					//int loc = (pos.x % REGIONSIZE) + REGIONSIZE * ((pos.y * REGIONSIZE) + REGIONSIZE * (pos.z));
					file.write(buffer, 1);
					ch->getByteData(buffer);
					//Comprimir buffer
					Byte compr[CHUNKSIZE*CHUNKSIZE*CHUNKSIZE];
					uLong comprLen = CHUNKSIZE * CHUNKSIZE * CHUNKSIZE;
					uLong srcLen = CHUNKSIZE * CHUNKSIZE * CHUNKSIZE;
					int err = compress(reinterpret_cast<Bytef*>(compr), &comprLen, reinterpret_cast<Bytef*>(buffer), srcLen);
					int size = (int)comprLen;
					file.write(reinterpret_cast<char*>(&size), sizeof(size));
					file.write(reinterpret_cast<char*>(compr), comprLen * sizeof(Byte));
					nChunks++;
					ch->setDirty(false);
				}
				else {
					buffer[0] = -1;
					file.write(buffer, 1);
					//file.write(zeros, chunkSize + 1);
				}
			}
		}
	}
	file.close();
	if (nChunks == 0) {
		std::remove(path.c_str());
	}
	return true;
}


bool World::loadFile(std::string path) {
	const int chunkSize = CHUNKSIZE * CHUNKSIZE * CHUNKSIZE;
	char buffer[chunkSize] = {};
	char zeros[chunkSize];
	memset(zeros, 0, chunkSize);
	std::fstream file;
	//std::string path = "worlds/" + this->name + "/chunks/" + fpath;
	size_t f1 = path.find("reg", 3);
	std::string s1 = path.substr(f1+3, std::string::npos);
	size_t f2 = s1.find("_", 1);
	int rX = atoi(s1.substr(0, f2).c_str());
	std::string s2 = s1.substr(f2 + 1, std::string::npos);
	size_t f3 = s2.find("_", 1);
	int rY = atoi(s2.substr(0, f3).c_str());
	std::string s3 = s2.substr(f3 + 1, std::string::npos);
	size_t f4 = s3.find(".", 1);
	int rZ = atoi(s3.substr(0, f4).c_str());
	Vector3<int> rPos = Vector3<int>(rX, rY, rZ);
	printf("Region %d %d %d\n", rPos.x, rPos.y, rPos.z);
	file.open(path.c_str(), std::ios::in | std::ios::binary);
	for (int x = rPos.x * REGIONSIZE; x < (rPos.x + 1) * REGIONSIZE; x++) {
		for (int y = rPos.y * REGIONSIZE; y < (rPos.y + 1) * REGIONSIZE; y++) {
			for (int z = rPos.z * REGIONSIZE; z < (rPos.z + 1) * REGIONSIZE; z++) {
				Vector3<int> cPos = Vector3<int>(x, y, z);
				file.read(buffer, 1); //bioma
				if (static_cast<int>(buffer[0]) == -1) { //El chunk és buit
					//estat[cPos] = ChunkState::LLEST;
					continue;
				}
				Bioma bio = static_cast<Bioma>(buffer[0]);
				int size;
				file.read(reinterpret_cast<char*>(&size), sizeof(int)); //Mida dades
				//printf("%d\n", size);

				char compr[CHUNKSIZE * CHUNKSIZE * CHUNKSIZE];
				file.read(compr, size);
				//Descomprimir buffer
				uLong comprLen = (long)size;
				uLong destLen = CHUNKSIZE * CHUNKSIZE * CHUNKSIZE;
				//printf("size: %lu\n", comprLen);
				if (uncompress(reinterpret_cast<Bytef*>(buffer), &destLen, reinterpret_cast<Bytef*>(compr), comprLen) == Z_DATA_ERROR) {
					//printf(":(\n");
				}

				Chunk* ch = new Chunk(this, cPos);
				chunks[cPos] = ch;
				ch->readFromByteData(buffer);
				ch->setBiome(bio);
				ch->updateMesh();
				updateNeighborChunks(cPos);
				estat[cPos] = ChunkState::LLEST;
			}
		}
	}
	file.close();
	return true;
}


bool World::loadRegion(Vector3<int> rPos) {
	std::string path = "worlds/" + this->name + "/chunks/reg" + std::to_string(rPos.x) + "-" + std::to_string(rPos.y) + "-" + std::to_string(rPos.z) + ".cnk";
	loadFile(path);
}

void World::redrawChunks() {
	std::map<Vector3<int>, Chunk*>::iterator cit;
	for (cit = chunks.begin(); cit != chunks.end(); cit++) {
		Vector3<int> cPos = cit->first;
		Chunk* ch = chunks[cPos];
		if (ch != nullptr && estat[cPos] == ChunkState::LLEST) {
			ch->updateMesh();
		}
	}
}

Vector3<int> World::getChunkPos(Vector3<int> bpos) {
	Vector3<int> toAdd = Vector3<int>(0, 0, 0);
	if (bpos.x < 0) {
		bpos.x++;
		toAdd = toAdd + Vector3<int>(-1, 0, 0);
	}
	if (bpos.y < 0) {
		bpos.y++;
		toAdd = toAdd + Vector3<int>(0, -1, 0);
	}
	if (bpos.z < 0) {
		bpos.z++;
		toAdd = toAdd + Vector3<int>(0, 0, -1);
	}
	return (bpos / CHUNKSIZE) + toAdd;
}

Vector3<int> World::getRegion(Vector3<int> cpos) {
	Vector3<int> toAdd = Vector3<int>(0, 0, 0);
	if (cpos.x < 0) {
		cpos.x++;
		toAdd = toAdd + Vector3<int>(-1, 0, 0);
	}
	if (cpos.y < 0) {
		cpos.y++;
		toAdd = toAdd + Vector3<int>(0, -1, 0);
	}
	if (cpos.z < 0) {
		cpos.z++;
		toAdd = toAdd + Vector3<int>(0, 0, -1);
	}
	return (cpos / REGIONSIZE + toAdd);
}