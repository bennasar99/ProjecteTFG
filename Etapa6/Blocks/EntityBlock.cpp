#include "EntityBlock.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

EntityBlock::EntityBlock(World* world, Bloc id, Vector3 pos) : Block(world, id, 0) {
    this->ent = 0;
    this->pos = pos;
    switch (id) { //Segons el bloc, inicialitzam el llum d'una manera o una altre
    case Bloc::NORIA:
        this->ent = world->addEntity(Entitat::NORIA, this->pos);
        break;
    case Bloc::GRUA:
        this->ent = world->addEntity(Entitat::GRUA, this->pos);
        break;
    }
}

void EntityBlock::destroy() {
    this->world->delEntity(this->ent); //Eliminam l'entitat del món
    this->ent = 0;
}

void EntityBlock::draw() {
    //Res, el món ja dibuixarà l'entitat
}