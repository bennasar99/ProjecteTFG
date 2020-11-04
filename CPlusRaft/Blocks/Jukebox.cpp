#include "Jukebox.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

Jukebox::Jukebox(World* world, Vector3 pos) : Block(world, Bloc::ALTAVEU, 0) {
    this->pos = pos;
}

void Jukebox::destroy() {
    
}

void Jukebox::interact() { //Reproduim música
    SoundManager::playSound(So::MUSICA, this->pos, true);
}