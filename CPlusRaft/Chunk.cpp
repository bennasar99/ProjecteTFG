#include "Chunk.h"
#include "World.h"

Chunk::Chunk(World* world, Vector3<int> pos) {
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int z = 0; z < CHUNKSIZE; z++) {
				//blocs[x][y][z] = nullptr;
				blocs[x][y][z] = Bloc::RES;
			}
		}
	}
	this->bio = Bioma::OCEA;
	this->world = world;
	this->cpos = pos;
	this->nblocs = 0;
}

void Chunk::drawO() {
	if (nblocs <= 0 && !Block::getMCEnabled()) {
		return;
	}
	if (firstdraw == true) {
		firstdraw = false;
		cMesh.updateO();
		this->updateTransparency(Vector3<float>(100, 100, 100));
		Vector3<int> cPos = this->getPos();
		//printf("First draw a %d %d %d\n", cPos.x, cPos.y, cPos.z);
	}
	glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::BLOC));
	glTranslatef(0.5f, 0.5f, 0.5f);
	if (Block::getMCEnabled()) {
		glFrontFace(GL_CW);
	}
	else {
		glFrontFace(GL_CCW);
	}
	cMesh.drawO();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Chunk::drawT() {
	if (nblocs <= 0) {
		return;
	}
	if (firstdraw == true) {
		firstdraw = false;
		this->updateTransparency(Vector3<float>(100, 100, 100));
		cMesh.update();
	}
	glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture(Textura::BLOC));
	glFrontFace(GL_CCW);
	glTranslatef(0.5f, 0.5f, 0.5f);
	glDepthMask(GL_FALSE);
	this->cMesh.drawT();
	glDepthMask(GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Vector3<int> Chunk::getPos() {
	return cpos;
}

bool Chunk::setBlock(Bloc bloc, Vector3<int> pos, bool overwrite) {
	//this->dirty = true;
	//Hi ha blocs amb les seves pr�pies classes, sin� s'utilitza la classe gen�rica
	//printf("GC %d %d %d\n", pos.x, pos.y, pos.z);
	if (this->blocs[pos.x][pos.y][pos.z] != Bloc::RES) {
		if (!overwrite) {
			return false;
		}
		/*this->blocs[pos.x][pos.y][pos.z]->destroy(this->world);
		delete this->blocs[pos.x][pos.y][pos.z];*/
		this->nblocs--;
	}
		
	this->blocs[pos.x][pos.y][pos.z] = bloc;
	this->nblocs++;

	return true;
}

Bloc Chunk::getBlock(Vector3<int> pos) {
	if (nblocs <= 0) {
		return Bloc::RES;
	}
	return blocs[pos.x][pos.y][pos.z];
}

void Chunk::update(float delta) {
	/*for (int x = 0; x < CHUNKSIZE; x++) {
			for (int z = 0; z < CHUNKSIZE; z++) {
				if (blocs[x][lastYupd][z] != 0) {
					blocs[x][lastYupd][z]->update(delta, this->world);
				}
			}
	}
	lastYupd++;
	lastYupd %= CHUNKSIZE;*/
}

//Destructor
Chunk::~Chunk() {
	//cMesh.erase();
	/*for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int z = 0; z < CHUNKSIZE; z++) {W
				if (blocs[x][y][z] != nullptr) {
					blocs[x][y][z]->destroy(this->world);
					delete blocs[x][y][z];
				}
			}
		}
	}*/
	//delete[] blocs;
}

//TODO actualitzar display list
bool Chunk::delBlock(Vector3<int> bpos, bool destroy) {
	printf("del %d %d %d\n", bpos.x, bpos.y, bpos.z);
  	if (this->blocs[bpos.x][bpos.y][bpos.z] != Bloc::RES) {      
		/*if (destroy) {
			this->blocs[bpos.x][bpos.y][bpos.z]->destroy(this->world);
		}*/
		/*delete this->blocs[bpos.x][bpos.y][bpos.z];*/
		this->blocs[bpos.x][bpos.y][bpos.z] = Bloc::RES;
		//this->den[bpos.x][bpos.y][bpos.z] = 2;
		nblocs--;
		this->updateMesh();
		world->updateNeighborChunks(this->cpos, bpos);
	}
	return true;
}

void Chunk::interact(Vector3<int> bpos) {
	/*if (blocs[bpos.x][bpos.y][bpos.z] == 0) {
		return;
	}
	blocs[bpos.x][bpos.y][bpos.z]->interact(this->world);*/
}

bool Chunk::isVisible(Vector3<int> bpos) {
	Vector3<int> pos = cpos * CHUNKSIZE + bpos;
	Vector3<int> toCheck[6] = { pos + Vector3<int>(1,0,0), pos - Vector3<int>(1,0,0), pos + Vector3<int>(0,1,0), pos - Vector3<int>(0,1,0),
						pos + Vector3<int>(0,0,1), pos - Vector3<int>(0,0,1) };
	for (int i = 0; i < 6; i++) {
		if (Block::isTransparent(getBlockWorld(toCheck[i]))) {
			return true;
		}
	}
	
	return false;
}

Bloc Chunk::getBlockWorld(Vector3<int> bpos) {
	if (bpos.x >= cpos.x * CHUNKSIZE && bpos.x < (cpos.x + 1) * CHUNKSIZE && bpos.y >= cpos.y * CHUNKSIZE &&
		bpos.y < (cpos.y + 1) * CHUNKSIZE && bpos.z >= cpos.z * CHUNKSIZE && bpos.z < (cpos.z + 1) * CHUNKSIZE) {
		Vector3<int> bposi = bpos % CHUNKSIZE;
		/*if (this->blocs[bposi.x][bposi.y][bposi.z] == nullptr) {
			return Bloc::RES;
		}*/
		if (bposi.x < 0 || bposi.y < 0 || bposi.z < 0) {
			return Bloc::RES;
		}
		return this->blocs[bposi.x][bposi.y][bposi.z];
	}
	else {
		return this->world->getBlock(bpos);
	}
}

bool Chunk::setBlockWorld(Bloc bloc, Vector3<int> bpos) {
	/*if (bpos.x >= world->size.x * CHUNKSIZE || bpos.y >= world->size.y * CHUNKSIZE || bpos.z >= world->size.z * CHUNKSIZE ||
		bpos.x < 0 || bpos.y < 0 || bpos.z < 0) {
		return false;
	}*/
	if (bpos.x >= cpos.x * CHUNKSIZE && bpos.x < (cpos.x + 1) * CHUNKSIZE && bpos.y >= cpos.y * CHUNKSIZE &&
		bpos.y < (cpos.y + 1) * CHUNKSIZE && bpos.z >= cpos.z * CHUNKSIZE && bpos.z < (cpos.z + 1) * CHUNKSIZE) {
		return this->setBlock(bloc, bpos % CHUNKSIZE);
	}
	else {
		return this->world->setBlock(bloc, bpos, false);
	}
}

bool Chunk::getByteData(char* arr) {
	int desp = 0;
	for (int y = 0; y < CHUNKSIZE; y++) {
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int x = 0; x < CHUNKSIZE; x++) {
				arr[desp++] = static_cast<unsigned char>(this->getBlock(Vector3<int>(x, y, z)));
			}
		}
	}
	return true;
}

bool Chunk::readFromByteData(char* arr) {
	int desp = 0;
	for (int y = 0; y < CHUNKSIZE; y++) {
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int x = 0; x < CHUNKSIZE; x++) {
				//printf("%d ", arr[desp]);
				Bloc tipus = static_cast<Bloc>(arr[desp++]);
				if (tipus != Bloc::RES) {
					this->setBlock(tipus, Vector3<int>(x, y, z), true); //No s'executa per qu� setBlock detecta que no est� llest
				}
			}
		}
	}
	this->updateMesh();
	return true;
}

void Chunk::updateMesh() {
	if (this->nblocs <= 0 && !Block::getMCEnabled()) { //CAs chunk que queda buit?
		return; //No cal fer res
	}
	cMesh.eraseO();
	std::list<dT> transparent;
	int nb = 0;
	for (int x = 0; x < CHUNKSIZE; x++) { //1a Passada: OPACS
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int y = 0; y < CHUNKSIZE; y++) {
				Bloc bt = blocs[x][y][z];
				if (bt == Bloc::RES && !Block::getMCEnabled()) {
					continue;
				}
				/*if (blocs[x][y][z] == Bloc::RES) {
					if (Block::getMCEnabled()) {
						bt = Bloc::RES;
					}
					else {
						continue;
					}
				}
				else {
					bt = blocs[x][y][z];
				}*/
				Vector3 bpos = Vector3<int>(x, y, z);
				if ((Block::isMarcheable(bt) || bt == Bloc::RES) && (Block::getMCEnabled())) {
					Block::drawMarching(bt, &this->cMesh, bpos, this);
					continue;
				}
				//Ordre: Esquerra, Damunt, Dreta, Abaix, Davant, Darrera
				Vector3<int> pos = cpos * CHUNKSIZE + bpos;
				Vector3<int> toCheck[6] = { pos - Vector3<int>(1,0,0), pos + Vector3<int>(0,1,0), pos + Vector3<int>(1,0,0), pos - Vector3<int>(0,1,0),
					pos + Vector3<int>(0,0,1), pos - Vector3<int>(0,0,1) };

				bool qualcun = false;
				bool visible[6] = { false, false, false, false, false, false };
				bool solid = Block::isSolid(bt);
				if (Block::isTransparent(bt)) {
					dT info = dT(Vector3<float>(pos.x, pos.y, pos.z));
					for (int i = 0; i < 6; i++) {
						Bloc bo = getBlockWorld(toCheck[i]);
						if (bo != bt && Block::canSeeThrough(bo) && !Block::isSolid(bo)) {
							info.visible[i] = true;
							qualcun = true;
						}
					}
					if (qualcun) {
						transparent.push_back(info);
					}
					if (Block::getMCEnabled() && !Block::isSolid(bt)) { //Dins els liquids transparents s'ha d'aplicar MC com si fos aire
						Block::drawMarching(Bloc::RES, &this->cMesh, bpos, this);
					}
				}
				else {
					for (int i = 0; i < 6; i++) {
						Bloc bt = getBlockWorld(toCheck[i]);
						if (Block::canSeeThrough(bt) || bt == Bloc::LIMIT) { //Aqu� dibuixam el l�mit dels chunks no carregats
							visible[i] = true;
							qualcun = true;
						}
					}
					if (qualcun) {
						//blocs[x][y][z]->draw(&cMesh, visible, Vector3<int>(x, y, z));
						Block::draw(blocs[x][y][z], &cMesh, bpos, visible);
					}
					if (Block::getMCEnabled() && Block::canSeeThrough(bt)) { //Amb els s�lids que no cobreixen totalment s'ha d'aplicar MC com si fos aire
						Block::drawMarching(Bloc::RES, &this->cMesh, bpos, this);
					}
				}
				nb++;
			}
		}
	}
	{
		const std::lock_guard<std::mutex> lock(mutex);
		this->transparent.clear();
		this->transparent = transparent;
	}
	this->firstdraw = true;
}

void Chunk::updateTransparency(Vector3<float> pPos){
	//Peta a la construcci� de la llista transparent local
	std::list<dT> transparent;
	{
		const std::lock_guard<std::mutex> lock(mutex);
		//if (!this->transparent.empty()) { //AIXO FA ERROR DE LA MAR perque A LO 1R t� algo a sa llista pes buits des chunk seg�ent i despr�s no i es cMesh no s'actualitza b�
			transparent = std::list<dT>(this->transparent);
		//}
	}
	/*if (transparent.empty()) { //I AIXO
		return;
	}*/
	cMesh.eraseT();
	transparent.sort([this,pPos](dT b1, dT b2) {
		//printf("dist %f\n", Vector3<float>::module(b1.pos - pPos));
		return (Vector3<float>::module(b1.pos - pPos) > Vector3<float>::module(b2.pos - pPos)); //de m�s enfora a + aprop
		});
	std::list<dT>::iterator it;
	Vector3<float> first;
	Vector3<float> last;
	for (it = transparent.begin(); it != transparent.end(); it++) {
		dT info = *it;
		if (it == transparent.begin()) {
			first = info.pos;
		}
		last = info.pos;
		int count = 0;
		for (int i = 0; i < 6; i++) {
			if (info.visible[i]) {
				count++;
			}
		}
		//printf("info %d %d %d count %d \n", info.pos.x, info.pos.y, info.pos.z, count);
		Vector3<int> bpos = Vector3<int>(info.pos.x, info.pos.y, info.pos.z) % CHUNKSIZE;
		if (blocs[bpos.x][bpos.y][bpos.z] != Bloc::RES) {
			//printf("%d %d %d, ", bpos.y, bpos.y, bpos.z);
			//blocs[bpos.x][bpos.y][bpos.z]->draw(&cMesh, info.visible, bpos);
			Block::draw(blocs[bpos.x][bpos.y][bpos.z], &cMesh, bpos, info.visible);
		}
	}
	cMesh.updateT();
	//if ((pPos / CHUNKSIZE).toInt() == this->getPos()) {
		//printf("first %d %d last %d %d pPos %d %d\n", (int)first.x, (int)first.z, (int)last.x, (int)last.z, (int)pPos.x, (int)pPos.z);
	//}
}

Bioma Chunk::getBiome() {
	return this->bio;
}

void Chunk::setBiome(Bioma biome) {
	this->bio = biome;
}

void Chunk::setDirty(bool dirty) {
	this->dirty = dirty;
}

bool Chunk::getDirty() {
	return this->dirty;
}