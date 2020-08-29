#include "LightBlock.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

LightBlock::LightBlock(World* world, Bloc id, Vector3 pos) : Block(world, id, 0) {
    this->light = 0;
    this->pos = pos;
    switch (id) { //Segons el bloc, inicialitzam el llum d'una manera o una altre
    case Bloc::TORXA: 
        this->light = world->addLight(pos + Vector3(0.5f,0.5f,0.5f));
        this->light->setLightConcentration(3.0f);
        this->light->setAttenuation(Attenuation::QUADRATIC, 1.0f);
        break;
    case Bloc::LLUMSOTIL:
        this->light = world->addLight(pos + Vector3(0, 0.5f,0));
        this->light->setDir(Vector3(0, -1, 0));
        this->light->setSpreadAngle(90.0f);
        this->light->setLightConcentration(5.0f);
        break;
    case Bloc::LLUMTERRA:
        this->light = world->addLight(pos - Vector3(0, 0.49f, 0));
        this->light->setDir(Vector3(0, 1, 0));
        this->light->setSpreadAngle(135.0f);
        this->light->setLightConcentration(4.0f);
        break;
    case Bloc::FAROLA:
        this->light = world->addLight(pos + Vector3(0, 1.f, 0));
        this->light->setLightConcentration(1.0f);
        this->light->setAttenuation(Attenuation::QUADRATIC, 0.5f);
        this->world->setBlock(Bloc::AIRE, pos + Vector3(0, 2, 0), this, false);
        this->world->setBlock(Bloc::AIRE, pos + Vector3(0, 1, 0), this, false);
        break;
    }
}

void LightBlock::destroy() { 
    this->world->delLight(this->light);
    this->light = 0;
    if (this->id == Bloc::FAROLA) {
        world->deleteBlock(pos + Vector3(0, 1, 0), false);
        world->deleteBlock(pos + Vector3(0, 2, 0), false);
        world->deleteBlock(pos, false);
    }
}

//Llum encesa ON/OFF
void LightBlock::interact() {
    SoundManager::playSound(So::ONOFF, this->pos, true);
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
    case Bloc::FAROLA: {
        glLineWidth(4.0f);

        //Tronc
        glColor3f(0.1f, 0.1f, 0.1f);
        glNormal3f(0, 1, 0);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.f);
        glBegin(GL_LINES);
        glVertex3f(-0, 1.75, 0);
        glVertex3f(-0, -0.5, 0);
        glEnd();

        //Base
        glColor3f(0.1f, 0.1f, 0.1f);
        glTranslatef(0, -0.5f, 0);
        glRotatef(-90.0f, 1.0f, 0.0f, 0);
        glutSolidCone(0.25f, 0.5f, 4, 1);
        glRotatef(90.0f, 1.0f, 0.0f, 0);

        //LLum
        glTranslatef(0, 2, 0);
        glDisable(GL_LIGHTING);
        glColor3f(1, 1, 0);
        glutSolidSphere(0.25, 100, 100);
        glEnable(GL_LIGHTING);
        break;
        }
    }
}