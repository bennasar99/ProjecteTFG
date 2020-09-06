#include "World.h"
#include "Object3D.h"

World::World(int sizex, int sizey, int sizez, Camera* camera)
{
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

	//Generador del món
	srand(1999); //Seed? xD
	Vector3 pos = Vector3(0, 0, 0);
	float lasty = (this->sizey * 16.0f) / 2.0f;
	for (pos.x = 0; pos.x < this->sizex * 16; pos.x++) {
		for (pos.z = 0; pos.z < this->sizez * 16; pos.z++) {
			for (pos.y = 0; pos.y <= lasty; pos.y++) {
				this->setBlock(Bloc::TERRA, pos, nullptr, false);
				if (pos.y == lasty){
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
							this->setBlock(Bloc::FUSTAARBRE, pos + Vector3(0,i,0), 0, false);
						}
						//Fulles
						int altura = rand() % 3 + 1;
						int amplada = (rand() % rand2)*2 + 1;
						for (int y = 0; y < altura; y++) {
							for (int x = -amplada; x <= amplada; x++) {
								for (int z = -amplada; z <= amplada; z++) {
									this->setBlock(Bloc::FULLAARBRE, pos + Vector3(0, rand2 + y, 0)
										+ Vector3(x,0,0) + Vector3(0,0,z), 0, false);
								}
							}
						}
					}
				}
			}
		}
		int random = rand() % 4;
		if (random == 2) {
			lasty++;
		}
		else if (random == 1) {
			lasty--;
		}
		if (lasty > this->sizey * 16 - 4) {
			lasty = this->sizey * 16 - 4;
		}
		if (lasty < 1) {
			lasty = 1;
		}
	}

	//Col·locam càmera
	int x = rand() % (sizex * 16);
	int z = rand() % (sizez * 16);
	for (int y = 0; y < this->sizey * 16.0f; y++) {
		if (getBlock(Vector3(x, y, z)) == Bloc::RES) {
			spawn = Vector3(x, y+1, z);
			break;
		}
	}

	//Fi generació món
	for (int i = 0; i < this->sizex * this->sizey * this->sizez; i++) {
		if (chunks[i] != nullptr) {
			chunks[i]->updateDL();
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
	std::list<Light*>::iterator it;
	for (it = lights.begin(); (it != lights.end()); it++) {
		(*it)->setDist(Vector3::module(pos - (*it)->getPosVec())); //Actualitzam la distància de les llums
	}
	lights.sort(compareLights); //Ordenam les llums per distància

	daytime += ((float)delta);
	if (daytime >= 86400.0f) {
		daytime = 0; //Reiniciam el temps del dia
		glEnable(sol); //Tornam activar el sol
	}
	else if (daytime > 43200.0f) { //Si es de vespre, desactivam el sol
		glDisable(sol);
	}
	
	//float dist = camera->getViewDist()/2; //Distància d'actualització dels blocs/estructures

	/*int x1 = (int)std::max(pos.x - dist, minpos.x), x2 = (int)std::min(pos.x + dist, maxpos.x);
	int y1 = (int)std::max(pos.y - dist, minpos.y), y2 = (int)std::min(pos.y + dist, maxpos.y);
	int z1 = (int)std::max(pos.z - dist, minpos.z), z2 = (int)std::min(pos.z + dist, maxpos.z);*/


	//Actualitzam els blocs dins els limits definits
	/*for (int x = x1; x <= x2; x++) {
		for (int y = y1; y <= y2; y++) {
			for (int z = z1; z <= z2; z++) {
				if (blocs[x + this->sizey * (y + this->sizez * z)] != 0) {
					blocs[x + this->sizey * (y + this->sizez * z)]->update(delta);
				}
			}
		}
	}*/

	//NOU CODI CHUNKS
	pos = pos / 16;
	pos.floor();
	float dist = floor(camera->getViewDist() / 16.0f);
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
	//for (int x = 0; x < this->sizex; x++) {
	//	for (int y = 0; y < this->sizey; y++) {
	//		for (int z = 0; z < this->sizez; z++) {
	//			blocs[x + this->sizey * (y + this->sizez * z)]->destroy();
	//			delete blocs[x + this->sizey * (y + this->sizez * z)];
	//			blocs[x + this->sizey * (y + this->sizez * z)] = 0;
	//		}
	//	}
	//}
 //   delete(this->blocs);
 //   this->blocs = NULL;

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
	//pos.noDecimals();
	//int desp = getDesp(pos);
	//if (desp == -1) {
	//	return false;
	//}
	//if (blocs[desp] != 0) {
	//	if (destroy) {
	//		blocs[desp]->destroy();
	//	}
	//	delete (blocs[desp]);
	//	blocs[desp] = 0;
	//}
	//return true;

	pos.floor();
	Vector3 cpos = pos / 16.0f;
	cpos.floor();
	Vector3 bpos = Vector3((int)pos.x % 16, (int)pos.y % 16, (int)pos.z % 16);
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
	Vector3 cpos = pos / 16.0f;
	cpos.floor();
	Vector3 bpos = Vector3((int)pos.x % 16, (int)pos.y % 16, (int)pos.z % 16);
	int desp = getDesp(cpos);
	if (desp == -1) {
		return false;
	}

	if (chunks[desp] == 0){
		chunks[desp] = new Chunk(this, cpos);
	}

	Block* bloc;
	switch (tipus) {
	case Bloc::PENDUL:
		bloc = new Pendul(this, pos);
		break;
	case Bloc::LLUMSOTIL: case Bloc::LLUMTERRA: case Bloc::TORXA: case Bloc::FAROLA:
		bloc = new LightBlock(this, tipus, pos);
		break;
	case Bloc::ALTAVEU:
		bloc = new Jukebox(this, pos);
		break;
	case Bloc::ESTALAGMITA:
		bloc = new SurfaceBlock(this, tipus);
		break;
	case Bloc::MIRALL:
		bloc = new Mirror(this, pos);
		break;
	case Bloc::NORIA: case Bloc::GRUA:
		bloc = new EntityBlock(this, tipus, pos);
		break;
	default:
		bloc = new Block(this, tipus, parent);
	}
	chunks[desp]->setBlock(bloc, bpos);
	if (listUpdate) {
		chunks[desp]->updateDL();
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
	Vector3 cpos = pos / 16.0f;
	cpos.floor();
	Vector3 bpos = Vector3((int)pos.x % 16, (int)pos.y % 16, (int)pos.z % 16);
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
		chunks[desp]->updateDL();
		updateNeighborChunks(cpos, bpos);
	}
}

//Obtenim el bloc a la posició indicada
Bloc World::getBlock(Vector3 pos) {
	pos.floor();
	Vector3 cpos = pos / 16.0f;
	cpos.floor();
	Vector3 bpos = Vector3((int)pos.x % 16, (int)pos.y % 16, (int)pos.z % 16);
	int desp = getDesp(cpos);
	if (desp == -1 || chunks[desp] == 0) {
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
	xmax = std::min(xmax, this->sizex*16);		ymax = std::min(ymax, this->sizey*16);		zmax = std::min(zmax, this->sizez*16);

	//NOU CODI CHUNKS
	Vector3 cMin = Vector3(xmin, ymin, zmin);
	cMin = cMin / 16.0f;
	cMin.floor();

	Vector3 cMax = Vector3(xmax, ymax, zmax);
	cMax = cMax / 16.0f;
	cMax.floor();

	//printf("%f %f %f, %f %f %f\n", cMin.x, cMin.y, cMin.z, cMax.x, cMax.y, cMax.z);

	int nchunks = 0;
	for (float x = cMin.x; x <= cMax.x; x++) {
		for (float y = cMin.y; y <= cMax.y; y++) {
			for (float z = cMin.z; z <= cMax.z; z++) {
				int desp = getDesp(Vector3(x, y, z));
				if (chunks[desp] == nullptr) {
					continue;
				}
				float dist = Vector3::module(camera->getPos() - Vector3(x * 16.0f, y * 16.0f, z * 16.0f));
				if ((dist < 24) || (camera->isVisible(Vector3(x * 16.0f, y * 16.0f, z * 16.0f), 100) ||
					camera->isVisible(Vector3(x * 16.0f + 16.0f, y * 16.0f, z * 16.0f), 100) ||
					camera->isVisible(Vector3(x * 16.0f, y * 16.0f, z * 16.0f + 16.0f), 100) ||
					camera->isVisible(Vector3(x * 16.0f + 16.0f, y * 16.0f, z * 16.0f + 16.0f), 100) ||
					camera->isVisible(Vector3(x * 16.0f, y * 16.0f + 16.0f, z * 16.0f), 100) ||
					camera->isVisible(Vector3(x * 16.0f + 16.0f, y * 16.0f + 16.0f, z * 16.0f), 100) ||
					camera->isVisible(Vector3(x * 16.0f, y * 16.0f + 16.0f, z * 16.0f + 16.0f), 100) ||
					camera->isVisible(Vector3(x * 16.0f + 16.0f, y * 16.0f + 16.0f, z * 16.0f + 16.0f), 100))){
					
					glPushMatrix();
					glTranslatef(x * 16.0f, y * 16.0f, z * 16.0f);
					chunks[desp]->draw();
					glPopMatrix();
					nchunks++;
				}
			}
		}
	}
	//printf("Chunks: %d\n", nchunks);
}

//Dibuixa un bloc a una posició determinada (Sense guardar-lo al món)
void World::drawBloc(Vector3 pos, Bloc tipus, bool wireframe) {
	glPushMatrix();
	pos.floor();
	Vector3 oldpos = pos;
	pos.floor();
	Vector3 front = camera->getFront();
	glTranslatef(pos.x+0.5f, pos.y+0.5f, pos.z+0.5f);
	Block bloc = Block(this, tipus, 0);
	bool visible[6] = { true, true, true, true, true, true };
	bloc.draw(wireframe, visible);
	bloc.destroy();
	glPopMatrix();
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
		glutSolidSphere(0.5f, 4, 4);
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
	Vector3 cpos = pos / 16.0f;
	cpos.floor();
	Vector3 bpos = Vector3((int)pos.x % 16, (int)pos.y % 16, (int)pos.z % 16);
	int desp = getDesp(cpos);
	if (desp == -1) {
		return;
	}
	return chunks[desp]->interact(bpos);
}

//Comprova que una posició valida i retorna el desplaçament corresponent a la posició
int World::getDesp(Vector3 pos) {
	int desp = (int)pos.x + this->sizey * ((int)pos.y + this->sizez * (int)pos.z);
	if ((desp >= (this->sizex * this->sizey * this->sizez))||(desp < 0) || pos.x < 0 || pos.y < 0 || pos.z < 0) {
		return -1;
	}
	return desp;
}

//Afegeix una entitat al món
Entity* World::addEntity(Entitat ent, Vector3 pos) {
	Entity* entitat = 0;
	switch (ent) {
	case Entitat::COTXE:
		entitat = new Car(this, pos, 0);
		break;
	case Entitat::COTXE2:
		entitat = new Car(this, pos, 1);
		break;
	case Entitat::GRUA:
		entitat = new Grua(this, pos);
		break;
	case Entitat::NORIA:
		entitat = new Noria(this, pos, 5, 0.1f, 8, 0.5f);
		break;
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
		chunks[desp]->updateDL();
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
		chunks[desp]->updateDL();
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
		chunks[desp]->updateDL();
	}
}

Vector3 World::getSpawn() {
	return this->spawn;
}