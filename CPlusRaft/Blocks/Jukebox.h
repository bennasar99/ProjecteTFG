#pragma once
#include "../Vector3.h"
#include "Block.h"
#include "../SoundManager.h"

/*
	Altaveu
*/
class Jukebox: public Block {
private:

	Vector3<int> pos;

public:
	Jukebox(Vector3<int> pos);

	void destroy();

	void interact();

};

#pragma once
