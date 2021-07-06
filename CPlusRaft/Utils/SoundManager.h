#pragma once
#include "al.h" 
#include "alc.h" 
#include <stdio.h>
#include "../Utils.h"


class SoundManager {
private:
	static unordered_map<string, ALuint> sons;

	static ALCcontext *context; //Contexte OpenAL
	static ALCdevice *device; //Dispositiu OpenAL

	SoundManager() {}
public:
	static bool loadSound(const char* path, string nom);
	static void playSound(string nom, Vector3<float> pos, bool forceStart);
	static bool initialize();
};