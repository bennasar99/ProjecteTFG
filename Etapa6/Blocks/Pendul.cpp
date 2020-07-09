#include "Pendul.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

const float g = 0.1f;
const float PI = 3.14159265358979f;

class World;

Pendul::Pendul(World* world, Vector3 pos) : Block(world, Bloc::PENDUL, 0) {
    this->pos = pos;

    // configurar els pènduls
    this->l1 = 0.5f;
    this->angle1 = 90;
    this->m1 = 10;
    this->w1 = 0;
    this->a1 = 0;

    this->l2 = 0.5f;
    this->angle2 = 90;
    this->m2 = 10;
    this->w2 = 0;
    this->a2 = 0;

    world->setBlock(Bloc::AIRE, pos + Vector3(1, 0, 0), this);
    world->setBlock(Bloc::AIRE, pos + Vector3(-1, 0, 0), this);
    world->setBlock(Bloc::AIRE, pos + Vector3(1, -1, 0), this);
    world->setBlock(Bloc::AIRE, pos + Vector3(-1, -1, 0), this);
    world->setBlock(Bloc::AIRE, pos + Vector3(0, -1, 0), this);
}

/**
  * Funció d'actualització de l'estat intern del pèndul
  */
void Pendul::update(int delta) {
    float tf = (float)delta / 1000.0f;

    float t1 = this->angle1 * PI / 180;
    float t2 = this->angle2 * PI / 180;

    float num1 = -g * (3 * m1 * m2) * sinf(t1);
    float num2 = -m2 * g * sinf(t1 - 2 * t2);
    float num3 = -2 * sinf(t1 - t2) * m2;
    float num4 = w2 * w2 * l2 + w1 * w1 * l1 * cosf(t1 - t2);
    float den = l1 * (2 * m1 + m2 - m2 * cosf(2 * t1 - 2 * t2));
    this->a1 = (num1 + num2 + num3 * num4) / den;

    num1 = 2 * sinf(t1 - t2);
    num2 = (w1 * w1 * l1 * (m1 + m2));
    num3 = g * (m1 + m2) * cosf(t1);
    num4 = w2 * w2 * l2 * m2 * cosf(t1 - t2);
    den = l2 * (2 * m1 + m2 - m2 * cosf(2 * t1 - 2 * t2));
    this->a2 = (num1 * (num2 + num3 + num4)) / den;

    this->w1 += this->a1 * tf;
    this->w2 += this->a2 * tf;

    t1 = fmodf(t1 + this->w1 * tf, 2 * PI);
    t2 = fmodf(t2 + this->w2 * tf, 2 * PI);;

    this->angle1 = t1 * 180.0f / PI;
    this->angle2 = t2 * 180.0f / PI;

    Vector3 tmp;
    tmp.x = sinf(t1) * l1 + sinf(t2) * l2;
    tmp.y = cosf(t1) * l1 - cosf(t2) * l2;
    tmp.z = 0;
    this->punts.push_back(tmp); //Introduim la posició actual a la llista de punts

    if (this->punts.size() > 1000) { //Alliberam espai a la llista
        this->punts.pop_front();
    }
}

//Funció de dibuixat del pèndul
void Pendul::draw(bool wireframe) {

    glPushMatrix();

    // dibuixar la base
    glutSolidCube(0.1f);

    // dibuixar la base del primer pèndul
    glRotatef(this->angle1 - 90, 0, 0, 1);
    glTranslatef(this->l1, 0, 0);
    glutSolidCube(0.2f);

    glPushMatrix();
    // dibuixar el segon pèndul
    glRotatef(this->angle2, 0, 0, 1);
    glRotatef(-this->angle1, 0, 0, 1);
    glTranslatef(this->l2, 0, 0);
    glutSolidCube(0.2f);


    // dibuixar línia entre els dos pènduls
    glColor3f(1, 0, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(-this->l2, 0, 0);
    glEnd();
    glPopMatrix();

    // dibuixar la línia entre la base i el pèndul
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(-this->l1, 0, 0);
    glEnd();
    glPopMatrix();

    if (this->penabled && this->punts.size() > 0) { //Dibuixam el camí del pèndul
        std::list<Vector3>::iterator it = this->punts.begin();
        Vector3 last = *it;
        it++;
        for (int i = 1; i < (int)this->punts.size(); i++) {
            Vector3 p = *it;

            glBegin(GL_LINES);
            glVertex3f(last.x, last.y, last.z);
            glVertex3f(p.x, p.y, p.z);
            glEnd();

            last = p;
            it++;
        }
    }
}

//Dibuixar camí pèndul ON/OFF
void Pendul::togglePoints() {
    this->penabled = !this->penabled;
}

void Pendul::destroy() { //Eliminam els blocs fills
    world->deleteBlock(pos + Vector3(1, 0, 0), false);
    world->deleteBlock(pos + Vector3(-1, 0, 0), false);
    world->deleteBlock(pos + Vector3(1, -1, 0), false);
    world->deleteBlock(pos + Vector3(-1, -1, 0), false);
    world->deleteBlock(pos + Vector3(0, -1, 0), false);
    world->deleteBlock(pos, false);
}