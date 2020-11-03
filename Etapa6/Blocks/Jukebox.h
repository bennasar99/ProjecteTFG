#pragma once
#include "../Vector3.h"
#include "Block.h"
#include "../SoundManager.h"

/*
	Altaveu
*/
class Jukebox: public Block {
private:

	Vector3 pos;

public:
	Jukebox(World* world, Vector3 pos);

	void destroy();

	void interact();

};

#pragma once
