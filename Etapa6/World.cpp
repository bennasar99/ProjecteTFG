#include "World.h"
#include "Object3D.h"

World::World(int sizex, int sizey, int sizez, Camera* camera)
{
	this->camera = camera;
	this->minpos = Vector3((float)sizex-1, (float)sizey-1, (float)sizez-1);
	//this->blocs = new Block*[(size_t)sizex * (size_t)sizey * (size_t)sizez];
 //   if (this->blocs != 0) { //S'ha reservat b� la mem�ria, inicialitzam tots els blocs a 0 (Bloc::RES)
 //       memset(this->blocs, 0, (size_t)sizex * (size_t)sizey * (size_t)sizez * sizeof(Block*));
 //   }

	//NOU CODI CHUNKS:
	this->chunks = new Chunk[sizex * sizey * sizez];

    this->sizex = sizex;
    this->sizey = sizey;
    this->sizez = sizez;

	//Omplim el m�n de terra i herbes
	Vector3 pos = Vector3(0, (float)(this->sizey / 2), 0);
	for (pos.x = 0 ; pos.x < this->sizex; pos.x++) {
		for (pos.z = 0; pos.z < this->sizez; pos.z++) {
			this->setBlock(Bloc::TERRA, pos);
			if ((rand() % 20) == 1) {
				this->setBlock(Bloc::HERBA, pos + Vector3(0, 1, 0));
			}
		}
	}

}

//Actualitzam els llums del m�n
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

//Afegeix un llum al m�n
Light* World::addLight(Vector3 pos) {
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
 * @param Vector3 posicio
 * Actualitza l'estat intern de les estructures del m�n, a m�s de la dist�ncia de les llums
 */
void World::update(int delta, Vector3 pos) {
	std::list<Light*>::iterator it;
	for (it = lights.begin(); (it != lights.end()); it++) {
		(*it)->setDist(Vector3::module(pos - (*it)->getPosVec())); //Actualitzam la dist�ncia de les llums
	}
	lights.sort(compareLights); //Ordenam les llums per dist�ncia

	daytime += ((float)delta);
	if (daytime >= 86400.0f) {
		daytime = 0; //Reiniciam el temps del dia
		glEnable(sol); //Tornam activar el sol
	}
	else if (daytime > 43200.0f) { //Si es de vespre, desactivam el sol
		glDisable(sol);
	}
	
	//float dist = camera->getViewDist()/2; //Dist�ncia d'actualitzaci� dels blocs/estructures

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
		for (int x = pos.x - dist; x < pos.x + dist; x++) {

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
	for (int x = 0; x < this->sizex; x++) {
		for (int y = 0; y < this->sizey; y++) {
			for (int z = 0; z < this->sizez; z++) {
				blocs[x + this->sizey * (y + this->sizez * z)]->destroy();
				delete blocs[x + this->sizey * (y + this->sizez * z)];
				blocs[x + this->sizey * (y + this->sizez * z)] = 0;
			}
		}
	}
    delete(this->blocs);
    this->blocs = NULL;

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
bool World::setBlock(Bloc tipus, Vector3 pos) {
	return this->setBlock(tipus, pos, 0);
}

//Eliminam el bloc de la posici� indicada
bool World::deleteBlock(Vector3 pos, bool destroy) { //Eliminar Bloc::RES?
	pos.noDecimals();
	int desp = getDesp(pos);
	if (desp == -1) {
		return false;
	}
	if (blocs[desp] != 0) {
		if (destroy) {
			blocs[desp]->destroy();
		}
		delete (blocs[desp]);
		blocs[desp] = 0;
	}
	return true;
}

//Col�locam un bloc d'un tipus determinat a la posici� indicada
bool World::setBlock(Bloc tipus, Vector3 pos, Block* parent) {
	//pos.noDecimals();
	//int desp = getDesp(pos);
	//if (desp == -1) {
	//	return false;
	//}

	////Actualitzam maxpos i minpos
	//maxpos.x = std::max(maxpos.x, pos.x); minpos.x = std::min(minpos.x, pos.x);
	//maxpos.y = std::max(maxpos.y, pos.y); minpos.y = std::min(minpos.y, pos.y);
	//maxpos.z = std::max(maxpos.z, pos.z); minpos.z = std::min(minpos.z, pos.z);

	////Hi ha blocs amb les seves pr�pies classes, sin� s'utilitza la classe gen�rica
	//switch (tipus) {
	//case Bloc::PENDUL:
	//	blocs[desp] = new Pendul(this, pos);
	//	break;
	//case Bloc::LLUMSOTIL: case Bloc::LLUMTERRA: case Bloc::TORXA: case Bloc::FAROLA:
	//	blocs[desp] = new LightBlock(this, tipus, pos);
	//	break;
	//case Bloc::ALTAVEU:
	//	blocs[desp] = new Jukebox(this, pos);
	//	break;
	//case Bloc::ESTALAGMITA:
	//	blocs[desp] = new SurfaceBlock(this, tipus);
	//	break;
	//case Bloc::MIRALL:
	//	blocs[desp] = new Mirror(this, pos);
	//	break;
	//case Bloc::NORIA: case Bloc::GRUA:
	//	blocs[desp] = new EntityBlock(this, tipus, pos);
	//	break;
	//default:
	//	blocs[desp] = new Block(this, tipus, parent);
	//}
	//return true;

	//NOU CODI CHUNKS:
	Vector3 cpos = pos / 16.0f;
	cpos.floor();
	Vector3 bpos = Vector3((int)pos.x % 16, (int)pos.y % 16, (int)pos.z % 16);
	int desp = getDesp(cpos);
	if (desp == -1) {
		return false;
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
	chunks[desp].setBlock(bloc, bpos);

	//Actualitzam maxpos i minpos
	maxpos.x = std::max(maxpos.x, cpos.x); minpos.x = std::min(minpos.x, cpos.x);
	maxpos.y = std::max(maxpos.y, cpos.y); minpos.y = std::min(minpos.y, cpos.y);
	maxpos.z = std::max(maxpos.z, cpos.z); minpos.z = std::min(minpos.z, cpos.z);

	return true;
}

//Assignam un bloc (per punter) a la posici� indicada
bool World::setBlock(Block* bloc, Vector3 pos) {
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

	return chunks[desp].setBlock(bloc, bpos);

	//pos.noDecimals();
	//int desp = getDesp(pos);
	//if (desp == -1) {
	//	return false;
	//}
	//maxpos.x = std::max(maxpos.x, pos.x); minpos.x = std::min(minpos.x, pos.x);
	//maxpos.y = std::max(maxpos.y, pos.y); minpos.y = std::min(minpos.y, pos.y);
	//maxpos.z = std::max(maxpos.z, pos.z); minpos.z = std::min(minpos.z, pos.z);
	//if (blocs[desp] != 0) {
	//	blocs[desp]->destroy();
	//	delete blocs[desp];
	//}
	//blocs[desp] = bloc;
	//return true;
}

//Obtenim el bloc a la posici� indicada
Bloc World::getBlock(Vector3 pos) {
	pos.noDecimals();
	int desp = getDesp(pos);
	if (desp != -1) {
		int desp = getDesp(pos);
		Block* block = blocs[desp];
		if (block != 0) {
			return block->getId();
		}
	}
	return Bloc::RES;
}

//remove indica si el bloc s'ha d'eliminar del m�n (NO S'ALLIBERA LA MEM�RIA)
Block* World::getBlockPointer(Vector3 pos, bool remove) {
	pos.noDecimals();
	Block* block = 0;
	int desp = getDesp(pos);
	if (desp != -1) {
		printf("%f %f %f, %d\n", pos.x, pos.y, pos.z, desp);
		if (blocs[desp] != 0) {
			printf("HOLA\n");
			block = blocs[desp];
		}
		if (remove) {
			blocs[desp] = 0;
		}
	}
	return block;
}

//Dibuixa el m�n visible
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

	//Obtenim el volum de possible visibilitat de la c�mera
	int xmin = camera->xmin;	int xmax = camera->xmax;
	int ymin = camera->ymin;	int ymax = camera->ymax;
	int zmin = camera->zmin;	int zmax = camera->zmax;

	xmin = std::max(xmin, (int)minpos.x);	ymin = std::max(ymin, (int)minpos.y);	zmin = std::max(zmin, (int)minpos.z);
	xmax = std::min(xmax, (int)maxpos.x);	ymax = std::min(ymax, (int)maxpos.y);	zmax = std::min(zmax, (int)maxpos.z);

	xmin = std::max(xmin, 0);				ymin = std::max(ymin, 0);				zmin = std::max(zmin, 0);
	xmax = std::min(xmax, this->sizex);		ymax = std::min(ymax, this->sizey);		zmax = std::min(zmax, this->sizez);

	////Dibuixam tots els blocs dins el volum de possible visibilitat de la c�mera
	//for (int x = xmin; x <= xmax; x++) {
	//	for (int y = ymin; y <= ymax; y++) {
	//		for (int z = zmin; z <= zmax; z++) {
	//			int desp = getDesp(Vector3((float)x, (float)y, (float)z));
	//			if (blocs[desp] != 0) {
	//				if (camera->isVisible(Vector3((float)x, (float)y, (float)z), 100)) {
	//					glPushMatrix();
	//					glTranslatef((float)x, (float)y, (float)z); //Ens translladam a la posici� del bloc
	//					blocs[desp]->draw(); //El dibuixam
	//					glPopMatrix();
	//				}
	//			}
	//		}
	//	}
	//}

	//NOU CODI CHUNKS
	Vector3 cMin = Vector3(xmin, ymin, zmin);
	cMin = cMin / 16.0f;
	cMin.floor();

	Vector3 cMax = Vector3(xmax, ymax, zmax);
	cMax = cMax / 16.0f;
	cMax.floor();
	for (int x = cMin.x; x < cMax.x; x++) {
		for (int y = cMin.y; y < cMax.y; y++) {
			for (int z = cMin.z; z < cMax.z; z++) {
				int desp = getDesp(Vector3(x, y, z));
				chunks[desp].draw();
			}
		}
	}
}

//Dibuixa un bloc a una posici� determinada (Sense guardar-lo al m�n)
void World::drawBloc(Vector3 pos, Bloc tipus, bool wireframe) {
	glPushMatrix();
	Vector3 oldpos = pos;
	pos.noDecimals();
	Vector3 front = camera->getFront();
	glTranslatef(pos.x, pos.y, pos.z);
	Block bloc = Block(this, tipus, 0);
	bloc.draw(wireframe);
	bloc.destroy();
	glPopMatrix();
}

//Dibuixa l'eix de coordenades a la posici� indicada
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

		//El sol no s'ha de veure afectat per la boira o l'il�luminaci�
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

//Interacci� amb un bloc
void World::interact(Vector3 pos) {
	pos.noDecimals();
	int desp = getDesp(pos);
	if (desp != -1){
		if (blocs[desp] != 0) {
			blocs[desp]->interact();
		}
	}
}

//Comprova que una posici� valida i retorna el despla�ament corresponent a la posici�
int World::getDesp(Vector3 pos) {
	int desp = (int)pos.x + this->sizey * ((int)pos.y + this->sizez * (int)pos.z);
	if ((desp > (this->sizex * this->sizey * this->sizez - 1))||(desp < 0)) {
		return -1;
	}
	return desp;
}

//Afegeix una entitat al m�n
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

//Elimina una entitat del m�n
void World::delEntity(Entity* ent) {
	ent->destroy();
	entities.remove(ent); //L'eliminam de la llista
	delete ent; //Alliberam la seva memoria
}

//Obt� l'entitat m�s propera a una posici� dins la dist�ncia (range) indicada, podent requerir que sigui controlable.
Entity* World::getNearestEntity(Vector3 pos, float range, bool controllable){
	Entity* entitat = 0;
	std::list<Entity*>::iterator ent;
	float currDist = range;
	for (ent = entities.begin(); (ent != entities.end()); ent++) {
		float dist = Vector3::module((*ent)->getPos() - pos);
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