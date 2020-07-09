#include "Noria.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../Utils.h"
#include <iostream>
#include <cmath>
#include "../World.h"

class World;

Noria::Noria(World* world, Vector3 pos, float radi, float velocitat, float vagons, float midaCabina) : Entity(world, pos) {

    this->radi = radi;
    this->velocitat = velocitat;
    this->vagons = vagons;
    this->angle = 0;
    this->midaCabina = midaCabina;

}

//Actualització de l'estat intern de la noria
void Noria::update(int delta) {
    this->angle += this->velocitat * delta; //Actualitzam la rotació
    if (this->angle > 360) {
        this->angle = 0;
    }
}

//Dibuixam la noria
void Noria::draw() {
    glTranslatef(0, radi + this->midaCabina - 0.5f, 0);
    glPushMatrix();
    float angle = toRad(this->angle);
    for (int i = 0; i < this->vagons; i++) {

        glPushMatrix();
        glColor3f(0, 0, 0);
        glBegin(GL_LINES); //Linia del centre al vagó
        glVertex3f(0, 0, 0);
        glVertex3f(this->radi * cosf(angle), this->radi * sinf(angle), 0);
        glEnd();

        glTranslatef(this->radi * cosf(angle), this->radi * sinf(angle), 0);
        dibuixaCabina(this->midaCabina); //Vagó
        glPopMatrix();

        glPushMatrix();
        glBegin(GL_LINES); //Connexió cap al pròxim vagó
        glVertex3f(this->radi * cosf(angle), this->radi * sinf(angle), 0);
        angle += toRad(360 / this->vagons);
        glVertex3f(this->radi * cosf(angle), this->radi * sinf(angle), 0);
        glEnd();

        glPopMatrix();

        //Suport d'enterra
        glColor3f(0, 0, 0);
        glLineWidth(3.0f);
        glBegin(GL_LINES); //Linia del centre al vagó
        glVertex3f(0, 0, 0);
        glVertex3f(this->radi/5, -this->radi - this->midaCabina, this->radi/5);

        glVertex3f(0, 0, 0);
        glVertex3f(-this->radi / 5, -this->radi - this->midaCabina, this->radi / 5);

        glVertex3f(0, 0, 0);
        glVertex3f(this->radi / 5, -this->radi - this->midaCabina, -this->radi / 5);

        glVertex3f(0, 0, 0);
        glVertex3f(-this->radi / 5, -this->radi - this->midaCabina, -this->radi / 5);
        glEnd();
        glLineWidth(1.0f);

    }
    glPopMatrix();
}

//Dibuixam una cabina de la mida indicada
void Noria::dibuixaCabina(float mida) {
    glBegin(GL_QUADS);

    glColor3f(1, 0, 0);

    glNormal3f(0, -1, 0); //Part d'abaix
    glVertex3f(-mida / 2, -mida / 2, mida / 2);
    glVertex3f(-mida / 2, -mida / 2, -mida / 2);
    glVertex3f(mida / 2, -mida / 2, -mida / 2);
    glVertex3f(mida / 2, -mida / 2, mida / 2);

    glNormal3f(0, 1, 0); //Part de dalt
    glVertex3f(-mida / 2, mida / 2, mida / 2);
    glVertex3f(mida / 2, mida / 2, mida / 2);
    glVertex3f(mida / 2, mida / 2, -mida / 2);
    glVertex3f(-mida / 2, mida / 2, -mida / 2);    

    glNormal3f(-1, -1, 0); //Costat esquerra abaix
    glVertex3f(-mida / 2, -mida / 2, mida / 2);
    glVertex3f((-mida / 2) * 1.3f, 0, mida / 2);
    glVertex3f((-mida / 2) * 1.3f, 0, -mida / 2);
    glVertex3f(-mida / 2, -mida / 2, -mida / 2);

    glNormal3f(1, -1, 0); //Costat dreta abaix
    glVertex3f((mida / 2) * 1.3f, 0, -mida / 2);
    glVertex3f((mida / 2) * 1.3f, 0, mida / 2);
    glVertex3f(mida / 2, -mida / 2, mida / 2);
    glVertex3f(mida / 2, -mida / 2, -mida / 2);

    glNormal3f(0, 0, 1); //Davant
    glVertex3f(-(mida / 2) * 1.3f, 0, mida / 2);
    glVertex3f(-mida / 2, -mida / 2, mida / 2);
    glVertex3f(mida / 2, -mida / 2, mida / 2);
    glVertex3f((mida / 2) * 1.3f, 0, mida / 2);

    glNormal3f(0, 0, -1); //Darrera
    glVertex3f(-(mida / 2) * 1.3f, 0, -mida / 2);
    glVertex3f((mida / 2) * 1.3f, 0, -mida / 2);
    glVertex3f(mida / 2, -mida / 2, -mida / 2);
    glVertex3f(-mida / 2, -mida / 2, -mida / 2);

    //Vidres

    glColor4f(0, 0, 1, 0.1f);
    glNormal3f(-1, 1, 0); //Costat esquerra adalt
    glVertex3f(-mida / 2, mida / 2, mida / 2);
    glVertex3f(-mida / 2, mida / 2, -mida / 2);
    glVertex3f((-mida / 2) * 1.3f, 0, -mida / 2);
    glVertex3f((-mida / 2) * 1.3f, 0, mida / 2);

    glNormal3f(1, 1, 0); //Costat dreta adalt
    glVertex3f(mida / 2, mida / 2, mida / 2);
    glVertex3f((mida / 2) * 1.3f, 0, mida / 2);
    glVertex3f((mida / 2) * 1.3f, 0, -mida / 2);
    glVertex3f(mida / 2, mida / 2, -mida / 2);

    glNormal3f(0, 0, 1); //Vidre davant
    glVertex3f(-mida / 2, mida / 2, mida / 2);
    glVertex3f(-(mida / 2) * 1.3f, 0, mida / 2);
    glVertex3f((mida / 2) * 1.3f, 0, mida / 2);
    glVertex3f(mida / 2, mida / 2, mida / 2);

    glNormal3f(0, 0, -1); //Vidre darrera
    glVertex3f(-mida / 2, mida / 2, -mida / 2);
    glVertex3f(mida / 2, mida / 2, -mida / 2);
    glVertex3f((mida / 2) * 1.3f, 0, -mida / 2);
    glVertex3f(-(mida / 2) * 1.3f, 0, -mida / 2);

    glEnd();

    //Linies
    glColor3f(1, 0, 0);
    glLineWidth(3.0f);
    glBegin(GL_LINES);

    glVertex3f(-mida / 2, mida / 2, -mida / 2);
    glVertex3f((-mida / 2) * 1.3f, 0, -mida / 2);

    glVertex3f(mida / 2, mida / 2, -mida / 2);
    glVertex3f((mida / 2) * 1.3f, 0, -mida / 2);

    glVertex3f(-mida / 2, mida / 2, mida / 2);
    glVertex3f((-mida / 2) * 1.3f, 0, mida / 2);

    glVertex3f(mida / 2, mida / 2, mida / 2);
    glVertex3f((mida / 2) * 1.3f, 0, mida / 2);

    glLineWidth(1.0f);
    glEnd();

}

void Noria::destroy() {

}