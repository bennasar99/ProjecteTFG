#include "Jukebox.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

Jukebox::Jukebox(Vector3<int> pos) : Block(Bloc::ALTAVEU) {
    this->pos = pos;
}

void Jukebox::destroy() {
    
}

void Jukebox::interact(World* world) { //Reproduim música
    SoundManager::playSound("Musica", Vector3<float>((float)this->pos.x, (float)this->pos.y, (float)this->pos.z), true);
}