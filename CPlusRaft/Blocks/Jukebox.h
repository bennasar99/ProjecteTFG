#pragma once
#include "../Utils/Vector3.h"
#include "Block.h"
#include "../Utils/SoundManager.h"

/*
	Altaveu
*/
class Jukebox: public Block {
private:

	Vector3<int> pos;

public:
	Jukebox(Vector3<int> pos);

	void destroy();

	void interact(World* world);

};

#pragma once
