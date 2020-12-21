#pragma once
#include "al.h" 
#include "alc.h" 
#include <stdio.h>
#include "Utils.h"


#define NSONS 5 //Nombre de sons

enum class So { //IDs dels sons
	ACCELERA,
	DESTRUEIX,
	MUSICA,
	COLOCA,
	ONOFF,
	CAMINA
};

class SoundManager {
private:
	static ALuint sons[NSONS]; //Array de sons
	static ALCcontext *context; //Contexte OpenAL
	static ALCdevice *device; //Dispositiu OpenAL

	SoundManager() {}
public:
	static bool loadSound(const char* path, So nom);
	static void playSound(So nom, Vector3<float> pos, bool forceStart);
	static bool initialize();
};