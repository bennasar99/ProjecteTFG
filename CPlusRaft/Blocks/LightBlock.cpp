#include "LightBlock.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

LightBlock::LightBlock(World* world, Bloc id, Vector3<int> pos) : Block(id) {
    this->light = 0;
    this->pos = pos;
    switch (id) { //Segons el bloc, inicialitzam el llum d'una manera o una altre
    case Bloc::TORXA: 
        this->light = world->addLight(Vector3<float>((float)pos.x, (float)pos.y, (float)pos.z) + Vector3<float>(0.5f,0.5f,0.5f));
        this->light->setLightConcentration(3.0f);
        this->light->setAttenuation(Attenuation::QUADRATIC, 1.0f);
        break;
    case Bloc::LLUMSOTIL:
        this->light = world->addLight(Vector3<float>((float)pos.x, (float)pos.y, (float)pos.z) + Vector3<float>(0, 0.5f,0));
        this->light->setDir(Vector3<float>(0, -1, 0));
        this->light->setSpreadAngle(90.0f);
        this->light->setLightConcentration(5.0f);
        break;
    case Bloc::LLUMTERRA:
        this->light = world->addLight(Vector3<float>((float)pos.x, (float)pos.y, (float)pos.z) - Vector3<float>(0, 0.49f, 0));
        this->light->setDir(Vector3<float>(0, 1, 0));
        this->light->setSpreadAngle(135.0f);
        this->light->setLightConcentration(4.0f);
        break;
    }
}

void LightBlock::destroy(World* world) {
    world->delLight(this->light);
    this->light = 0;
}

//Llum encesa ON/OFF
void LightBlock::interact(World* world) {
    SoundManager::playSound(So::ONOFF, this->light->getPosVec(), true);
    this->light->setEnabled(!this->light->getEnabled());
}

//Funció de dibuixat (Del bloc/objecte tal com és, no icona)
void LightBlock::draw() {
    switch (this->id) {
    case Bloc::TORXA: //torxa
        glDisable(GL_LIGHTING);
        glLineWidth(3.0f);
        glColor3f(0.5f, 0.35f, 0.05f);
        glBegin(GL_LINES);
        glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, -0.5, 0.0); //Cos
        glEnd();
        glColor3f(1, 1, 0);
        glBegin(GL_LINES);
        glVertex3d(0.0, 0.2, 0.0); glVertex3d(0.0, 0.1, 0.0); //Punta
        glEnd();
        glLineWidth(1.0f);
        glEnable(GL_LIGHTING);
        break;
    case Bloc::LLUMSOTIL: //Llum de sòtil
        glDisable(GL_LIGHTING);
        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
        glVertex3d(0.5, 0.49, 0.5); glVertex3d(0.5, 0.49, -0.5); glVertex3d(-0.5, 0.49, -0.5); glVertex3d(-0.5, 0.49, 0.5);
        glColor3f(0, 0, 0);
        glLineWidth(2.0f);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex3d(0.5, 0.49, 0.5); glVertex3d(0.5, 0.49, -0.5); glVertex3d(-0.5, 0.49, -0.5); glVertex3d(-0.5, 0.49, 0.5); glVertex3d(0.5, 0.49, 0.5);
        glEnd();
        glEnable(GL_LIGHTING);
        break;
    case Bloc::LLUMTERRA: //Foco (llum de sòtil però de baix cap adalt)
        glDisable(GL_LIGHTING);
        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
        glVertex3d(0.5, -0.49, 0.5); glVertex3d(0.5, -0.49, -0.5); glVertex3d(-0.5, -0.49, -0.5); glVertex3d(-0.49, -0.5, 0.5);
        glColor3f(0, 0, 0);
        glLineWidth(2.0f);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex3d(0.5, -0.49, 0.5); glVertex3d(0.5, -0.49, -0.5); glVertex3d(-0.5, -0.49, -0.5); glVertex3d(-0.5, -0.49, 0.5); glVertex3d(0.5, -0.49, 0.5);
        glEnd();
        glEnable(GL_LIGHTING);
        break;
    }
}