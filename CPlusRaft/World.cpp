#include "World.h"


World::World(int seed, int sizex, int sizey, int sizez, Camera* camera)
{
	this->br = new BlockRenderer();

	this->noise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
	this->noise.SetFrequency(0.01f);
	this->noise.SetFractalGain(0.8f);
	this->noise.SetSeed(seed);

	this->seed = seed;
	this->camera = camera;
	this->minpos = Vector3((float)sizex - 1, (float)sizey - 1, (float)sizez - 1);

	//NOU CODI CHUNKS:
	this->chunks = new Chunk * [(size_t)sizex * (size_t)sizey * (size_t)sizez];
	for (int i = 0; i < sizex * sizey * sizez; i++) {
		this->chunks[i] = nullptr;
	}

	this->sizex = sizex;
	this->sizey = sizey;
	this->sizez = sizez;

	this->generate(seed);

	//Col·locam càmera
	int x = rand() % (sizex * CHUNKSIZE);
	int z = rand() % (sizez * CHUNKSIZE);
	for (int y = 0; y < this->sizey * CHUNKSIZE; y++) {
		if (getBlock(Vector3(x, y, z)) == Bloc::RES) {
			spawn = Vector3(x, y + 1, z);
			break;
		}
	}
}

World::World(std::string name, Camera* camera) {
	this->br = new BlockRenderer();
	this->seed = 0;

	this->noise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
	this->noise.SetFrequency(0.01f);
	this->noise.SetFractalGain(0.8f);
	this->noise.SetSeed(seed);

	this->camera = camera;
	this->minpos = Vector3((float)sizex - 1, (float)sizey - 1, (float)sizez - 1);

	std::fstream file;
	file.open("worlds/" + name, std::ios::in | std::ios::binary);

	//Mida del món
	char buff[sizeof(int)];
	file.read(buff, 3);
	this->sizex = int(buff[0]);
	this->sizey = int(buff[1]);
	this->sizez = int(buff[2]);
	file.read(buff, 3);
	int sX = unsigned char(buff[0]);
	int sY = unsigned char(buff[1]);
	int sZ = unsigned char(buff[2]);
	this->spawn = Vector3((float)sX, (float)sY, (float)sZ);
	//file.read(buff, sizeof(int));
	//this->seed = int(buff);

	this->chunks = new Chunk * [(size_t)sizex * (size_t)sizey * (size_t)sizez];

	printf("Mides del mon: %d %d %d, spawn a: %d %d %d. Seed %d\n", this->sizex, this->sizey, this->sizez, sX, sY, sZ, this->seed);

	const int chunkSize = CHUNKSIZE * CHUNKSIZE * CHUNKSIZE;
	char buffer[chunkSize];
	for (int x = 0; x < this->sizex; x++) {
		for (int y = 0; y < this->sizey; y++) {
			for (int z = 0; z < this->sizez; z++) {
				//printf("Chunk a %d %d %d...\n", x, y, z);
				int desp = getDesp(Vector3(x, y, z));
				file.read(buffer, chunkSize);
				chunks[desp] = new Chunk(this, Vector3(x,y,z));
				chunks[desp]->readFromByteData(buffer);
				//printf("LLEGIT\n");
			}
		}
	}
	printf("Món llegit");
	fflush(stdout);;

	file.close();

	//Fi generació món
	for (int i = 0; i < this->sizex * this->sizey * this->sizez; i++) {
		if (chunks[i] != nullptr) {
			//chunks[i]->updateDL();
			//chunks[i]->updateMesh();
		}
	}
}

void World::save(std::string name) {
	std::fstream file;
	file.open("worlds/" + name, std::ios::out | std::ios::binary);

	//Guardam mides i spawn
	char sX = this->sizex;
	char sY = this->sizey;
	char sZ = this->sizez;
	file.write(&sX, 1);
	file.write(&sY, 1);
	file.write(&sZ, 1);
	sX = (int)this->spawn.x;
	sY = (int)this->spawn.y;
	sZ = (int)this->spawn.z;
	file.write(&sX, 1);
	file.write(&sY, 1);
	file.write(&sZ, 1);
	//char seed = this->seed;
	//file.write(&seed, 1);

	const int chunkSize = CHUNKSIZE* CHUNKSIZE* CHUNKSIZE;
	char buffer[chunkSize];

	for (int x = 0; x < this->sizex; x++) {
		for (int y = 0; y < this->sizey; y++) {
			for (int z = 0; z < this->sizez; z++) {
				int desp = getDesp(Vector3(x, y, z));
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
	//for (int i = 0; i < this->sizex * this->sizey * this->sizez; i++) {
	//	if (chunks[i] != nullptr) {
	//		chunks[i]->getByteData(buffer);
	//		file.write(buffer, chunkSize);
	//	}
	//	else {
	//		char zeros[chunkSize];
	//		file.write(zeros, chunkSize);
	//	}
	//}

	file.close();
}

void World::generate(int seed) { //TODO: guardar spawn a world
	//Generador del món
	srand(seed); //Seed? xD
	noise.SetSeed(seed);
	Vector3 pos = Vector3(0, 0, 0);
	float lasty = 0; // = (this->sizey * CHUNKSIZE) / 2.0f;
	for (pos.x = 0; pos.x < (this->sizex * CHUNKSIZE); pos.x++) {
		for (pos.z = 0; pos.z < (this->sizez * CHUNKSIZE); pos.z++) {
			lasty = (this->sizey*CHUNKSIZE)/2 + noise.GetNoise(pos.x, pos.z) * 80;
			lasty = std::min(lasty, (float)this->sizey * CHUNKSIZE);
			//printf("last: %f\n", lasty);
			for (pos.y = 0; pos.y < lasty; pos.y++) {
				this->setBlock(Bloc::TERRA, pos, nullptr, false);
				if (pos.y == (int)lasty) {
					if (lasty > (this->sizey * CHUNKSIZE) / 2) { //Elements superfície
						int random = rand() % 128;
						if (random == 1 || random == 5) {
							this->setBlock(Bloc::HERBA, pos + Vector3(0, 1, 0), 0, false);
						}
						else if (random == 2 || random == 6) {
							this->setBlock(Bloc::HERBAFULL, pos + Vector3(0, 1, 0), 0, false);
							this->setBlock(Bloc::HERBA, pos + Vector3(0, 2, 0), 0, false);
						}
						else if (random == 3) {
							this->setBlock(Bloc::AIGUA, pos, 0, false);
						}
						else if (random == 4) {
							//Tronc
							this->setBlock(Bloc::FUSTAARBRE, pos, 0, false);
							int rand2 = rand() % 5 + 1;
							for (int i = 1; i <= rand2; i++) {
								this->setBlock(Bloc::FUSTAARBRE, pos + Vector3(0, i, 0), 0, false);
							}
							//Fulles
							int altura = rand() % 3 + 1;
							int amplada = (rand() % (rand2)) + 1;
							for (int y = 0; y < altura; y++) {
								for (int x = -amplada; x <= amplada; x++) {
									for (int z = -amplada; z <= amplada; z++) {
										this->setBlock(Bloc::FULLAARBRE, pos + Vector3(0, rand2 + y, 0)
											+ Vector3(x, 0, 0) + Vector3(0, 0, z), 0, false);
									}
								}
							}
						}
					}
				}
			}
			//Oceans
			for (pos.y = lasty; pos.y <= (this->sizey * CHUNKSIZE)/2; pos.y++) {
				this->setBlock(Bloc::AIGUA, pos, nullptr, false);
			}
		}
	}


}

//Actualitzam els llums del món
void World::updateLights(Vector3 camPos, Vector3 front, float camFovY, float aspect) {
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
			if ((camera->isVisible((*it)->getPosVec(), 0)) || ((*it)->getDist() <= 1.0f)) { //Mostram només les llums visibles o molt properes
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
	glPopMatrix();

	//Sol, establim la seva posició segons el moment del dia
	this->solpos = Vector3(cosf((daytime / 43200.0f) * 3.1416f), sinf((daytime / 43200.0f) * 3.1416f), 0.0f);
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

//Afegeix un llum al món
Light* World::addLight(Vector3 pos) {
	Light* llum = new Light(pos);
	this->lights.push_front(llum);
	return llum;
}

//Elimina una llum del món
void World::delLight(Light* light) {
	this->lights.remove(light);
	delete light;
}

//Comparador per la llista de llums
static bool compareLights(Light* a, Light* b) {
	return a->getDist() < b->getDist();
}

/**
 * Actualització del món a cada frame (Idle)
 * @param delta temps des de la darrera actualització en ms
 * @param Vector3 posicio
 * Actualitza l'estat intern de les estructures del món, a més de la distància de les llums
 */
void World::update(int delta, Vector3 pos) {

	daytime += ((float)delta);
	if (daytime >= 86400.0f) {
		daytime = 0; //Reiniciam el temps del dia
		glEnable(sol); //Tornam activar el sol
	}
	else if (daytime > 43200.0f) { //Si es de vespre, desactivam el sol
		glDisable(sol);
	}
	
	updTimer -= delta;
	if (updTimer > 0) {
		return;
	}
	updTimer = 1000; //10 tps
	//Ordenació llums
	std::list<Light*>::iterator it;
	for (it = lights.begin(); (it != lights.end()); it++) {
		(*it)->setDist(Vector3::module(pos - (*it)->getPosVec())); //Actualitzam la distància de les llums
	}
	lights.sort(compareLights); //Ordenam les llums per distància

	//NOU CODI CHUNKS
	pos = pos / CHUNKSIZE;
	pos.floor();
	float dist = floor(camera->getViewDist() / CHUNKSIZE);
	for (int x = pos.x - dist; x < pos.x + dist; x++) {
		for (int y = pos.y - dist; y < pos.y + dist; y++) {
			for (int z = pos.z - dist; z < pos.z + dist; z++) {
				int desp = getDesp(Vector3(x, y, z));
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

//Alliberam la memòria de tots els blocs i entitats
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

//Col·locam un bloc a la posició indicada
bool World::setBlock(Bloc tipus, Vector3 pos) {
	return this->setBlock(tipus, pos, 0, true);
}

//Eliminam el bloc de la posició indicada
bool World::deleteBlock(Vector3 pos, bool destroy) { //Eliminar Bloc::RES?

	pos.floor();
	Vector3 cpos = pos / CHUNKSIZE;
	cpos.floor();
	Vector3 bpos = Vector3((int)pos.x % CHUNKSIZE, (int)pos.y % CHUNKSIZE, (int)pos.z % CHUNKSIZE);
	int desp = getDesp(cpos);
	if (desp == -1) {
		return false;
	}
	return this->chunks[desp]->delBlock(bpos, true);
}

//Col·locam un bloc d'un tipus determinat a la posició indicada
bool World::setBlock(Bloc tipus, Vector3 pos, Block* parent, bool listUpdate) {

	//NOU CODI CHUNKS:
	pos.floor();
	Vector3 cpos = pos / CHUNKSIZE;
	cpos.floor();
	Vector3 bpos = Vector3((int)pos.x % CHUNKSIZE, (int)pos.y % CHUNKSIZE, (int)pos.z % CHUNKSIZE);
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
	case Bloc::ALTAVEU:
		bloc = new Jukebox(this, pos);
		break;
	default:
		bloc = new Block(this, tipus, parent);
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

//Assignam un bloc (per punter) a la posició indicada
bool World::setBlock(Block* bloc, Vector3 pos, bool listUpdate) {
	pos.floor();
	Vector3 cpos = pos / CHUNKSIZE;
	cpos.floor();
	Vector3 bpos = Vector3((int)pos.x % CHUNKSIZE, (int)pos.y % CHUNKSIZE, (int)pos.z % CHUNKSIZE);
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

//Obtenim el bloc a la posició indicada
Bloc World::getBlock(Vector3 pos) {
	pos.floor();
	Vector3 cpos = pos / CHUNKSIZE;
	cpos.floor();
	Vector3 bpos = Vector3((int)pos.x % CHUNKSIZE, (int)pos.y % CHUNKSIZE, (int)pos.z % CHUNKSIZE);
	int desp = getDesp(cpos);
	if (desp == -1) {
		return Bloc::LIMIT;
	}
	if (chunks[desp] == 0) {
		return Bloc::RES;
	}
	return chunks[desp]->getBlock(bpos); 
}

//remove indica si el bloc s'ha d'eliminar del món (NO S'ALLIBERA LA MEMÒRIA)
Block* World::getBlockPointer(Vector3 pos, bool remove) {
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
void World::draw(Vector3 pos, float dist) {

	camera->setViewDist(dist);

	std::list<Entity*>::iterator ent;
	for (ent = entities.begin(); (ent != entities.end()); ent++) {
		Vector3 pos = (*ent)->getPos();
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		(*ent)->draw();
		glPopMatrix();
	}

	//Obtenim el volum de possible visibilitat de la càmera
	int xmin = camera->xmin;	int xmax = camera->xmax;
	int ymin = camera->ymin;	int ymax = camera->ymax;
	int zmin = camera->zmin;	int zmax = camera->zmax;

	xmin = std::max(xmin, (int)minpos.x);	ymin = std::max(ymin, (int)minpos.y);	zmin = std::max(zmin, (int)minpos.z);
	xmax = std::min(xmax, (int)maxpos.x);	ymax = std::min(ymax, (int)maxpos.y);	zmax = std::min(zmax, (int)maxpos.z);

	xmin = std::max(xmin, 0);				ymin = std::max(ymin, 0);				zmin = std::max(zmin, 0);
	xmax = std::min(xmax, this->sizex* CHUNKSIZE);		ymax = std::min(ymax, this->sizey* CHUNKSIZE);		zmax = std::min(zmax, this->sizez* CHUNKSIZE);

	//NOU CODI CHUNKS
	Vector3 cMin = Vector3(xmin, ymin, zmin);
	cMin = cMin / CHUNKSIZE;
	cMin.floor();

	Vector3 cMax = Vector3(xmax, ymax, zmax);
	cMax = cMax / CHUNKSIZE;
	cMax.floor();

	//printf("%f %f %f, %f %f %f\n", cMin.x, cMin.y, cMin.z, cMax.x, cMax.y, cMax.z);
	int nchunks = 0;
	for (float x = cMin.x; x <= cMax.x; x++) { //Optimització possible: si els chunks circumdants no son visibles, aquest no ho és
		for (float y = cMin.y; y <= cMax.y; y++) {
			for (float z = cMin.z; z <= cMax.z; z++) {
				int desp = getDesp(Vector3(x, y, z));
				if (chunks[desp] == nullptr) {
					continue;
				}
				float dist = Vector3::module(camera->getPos() - Vector3(x * CHUNKSIZE + CHUNKSIZE/2, y * CHUNKSIZE + CHUNKSIZE/2, z * CHUNKSIZE + CHUNKSIZE/2));
				if ((dist < CHUNKSIZE) || (camera->isVisible(Vector3(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100))){
					
					glPushMatrix();
					glTranslatef(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE);
					chunks[desp]->drawO();
					glPopMatrix();
					nchunks++;
				}
			}
		}
	}
	for (float x = cMin.x; x <= cMax.x; x++) { //Optimització possible: si els chunks circumdants no son visibles, aquest no ho és
		for (float y = cMin.y; y <= cMax.y; y++) {
			for (float z = cMin.z; z <= cMax.z; z++) {
				int desp = getDesp(Vector3(x, y, z));
				if (chunks[desp] == nullptr) {
					continue;
				}
				float dist = Vector3::module(camera->getPos() - Vector3(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE));
				if ((dist < 24) || (camera->isVisible(Vector3(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100) ||
					camera->isVisible(Vector3(x * CHUNKSIZE + CHUNKSIZE, y * CHUNKSIZE + CHUNKSIZE, z * CHUNKSIZE + CHUNKSIZE), 100))) {

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

//Dibuixa un bloc a una posició determinada (Sense guardar-lo al món)
void World::drawBloc(Bloc tipus) {
	br->drawBloc(tipus);
}

//Dibuixa l'eix de coordenades a la posició indicada
void World::drawAxis(Vector3 pos, float axisSize) { 
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
void World::drawSol(Vector3 pos, float dist) {
	if (daytime >= 0.0f && daytime <= 43200.0f) {
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
		glClearColor(solpos.y, solpos.y, solpos.y, 1);

		glPopMatrix();
	}
}

//Interacció amb un bloc
void World::interact(Vector3 pos) {
	pos.floor();
	Vector3 cpos = pos / CHUNKSIZE;
	cpos.floor();
	Vector3 bpos = Vector3((int)pos.x % CHUNKSIZE, (int)pos.y % CHUNKSIZE, (int)pos.z % CHUNKSIZE);
	int desp = getDesp(cpos);
	if (desp == -1) {
		return;
	}
	return chunks[desp]->interact(bpos);
}

//Comprova que una posició valida i retorna el desplaçament corresponent a la posició
int World::getDesp(Vector3 pos) {
	int desp = (int)pos.x + this->sizey * ((int)pos.y + this->sizez * (int)pos.z);
	if ((desp >= (this->sizex * this->sizey * this->sizez))||(desp < 0) || pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= this->sizex || pos.y >= this->sizey || pos.z >= this->sizez) {
		return -1;
	}
	return desp;
}

//Afegeix una entitat al món
Entity* World::addEntity(Entitat ent, Vector3 pos) {
	Entity* entitat = 0;
	switch (ent) {
		//Ficar entitat amb new a *entitat
	}
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
Entity* World::getNearestEntity(Vector3 pos, float range, bool controllable){
	Entity* entitat = 0;
	std::list<Entity*>::iterator ent;
	float currDist = range;
	for (ent = entities.begin(); (ent != entities.end()); ent++) {
		float dist = Vector3::module((*ent)->getPos() - pos);
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

void World::updateNeighborChunks(Vector3 cpos, Vector3 bpos) {
	int desp;
	Vector3 ncpos;
	if (bpos.x == 15) {
		ncpos = cpos + Vector3(1, 0, 0);
	}
	else if (bpos.x == 0) {
		ncpos = cpos - Vector3(1, 0, 0);
	}
	desp = getDesp(ncpos);
	if (desp != -1) {
		if (chunks[desp] == nullptr) {
			chunks[desp] = new Chunk(this, ncpos);
		}
		chunks[desp]->updateMesh();
	}
	if (bpos.y == 15) {
		ncpos = cpos + Vector3(0, 1, 0);
	}
	else if (bpos.y == 0) {
		ncpos = cpos - Vector3(0, 1, 0);
	}
	desp = getDesp(ncpos);
	if (desp != -1) {
		if (chunks[desp] == nullptr) {
			chunks[desp] = new Chunk(this, ncpos);
		}
		chunks[desp]->updateMesh();
	}
	if (bpos.z == 15) {
		ncpos = cpos + Vector3(0, 0, 1);
	}
	else if (bpos.z == 0) {
		ncpos = cpos - Vector3(1, 0, 0);
	}
	desp = getDesp(ncpos);
	if (desp != -1) {
		if (chunks[desp] == nullptr) {
			chunks[desp] = new Chunk(this, ncpos);
		}
		chunks[desp]->updateMesh();
	}
}

Vector3 World::getSpawn() {
	return this->spawn;
}