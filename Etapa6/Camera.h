#pragma once

#include "Vector3.h"
#include <GL/freeglut.h>
#include <list>
#include "Utils.h"
#include <algorithm>
#include <al.h>

class Camera {
private:

	Vector3 pos;
	Vector3 worldUp = Vector3(0, 1, 0); //up del m�n (sempre aquest)
	Vector3 front = Vector3(0, 0, -1); //Indica on apunta la c�mera
	Vector3 up = Vector3(0, 1, 0); //Up de la c�mera (variable)
	Vector3 right = Vector3(0, 0, 1); //Perpendicular a worldUp i front
	Vector3 camRight = Vector3(1, 0, 0); //Right de la c�mera

	float fov = 60.0f; //Angle de visi� (eix Y) de la c�mera
	float aspect = 1.0f; //Aspecte (width/height)
	float viewDist = 64.0f; //Dist�ncia de visi�
	float sensitivity = 0.1f; //Sensibilitat de la c�mera als moviments del ratol�
	float speed = 0.005f; //Velocitat de moviment (Despla�ament)

	bool freeMove = true; //Moviment (posici�) lliure perm�s
	bool freeLook = true; //Orientaci� lliure permesa
	bool drawMove = true; //Dibuix del cam� de la c�mera activat

	std::list<Vector3> punts; //Posicions anteriors

public:
	int xmin, ymin, zmin, xmax, ymax, zmax; //Punts de l'�rea visible definida per la c�mera

	enum class direction { //Direccions de moviment
		ENVANT,
		ENRERE,
		ESQUERRA,
		DRETA,
		ADALT,
		ABAIX
	};

	enum class angle { //Angles
		NADIR,
		CONTRAPICAT,
		NORMAL,
		PICAT,
		CENITAL
	};

	enum class pla { //Plans
		PLANTA,
		AL�AT,
		PERFIL,
		PERSPECTIVA
	};

	void updateVec();
	void updateCorners();

	Vector3 getPos();
	void setPos(Vector3 pos);

	Vector3 getFront();
	void setFront(Vector3 front);

	Vector3 getRight();
	void setRight(Vector3 right);

	void setFov(float fov);
	float getFov();

	void setAspect(float aspect);
	float getAspect();

	void setViewDist(float viewDist);
	float getViewDist();

	bool getFreeMove();
	void setFreeMove(bool set);

	bool getFreeLook();
	void setFreeLook(bool set);

	bool getDrawMove();
	void setDrawMove(bool set);

	void lookAt(Vector3 pos);
	void zoom(float zoom);

	float pitch = 0.0f; //Adalt o abaix
	float yaw = -90.0f; //Esquerra o dreta, en aquest valor mira de primeres cap a Z -

	void lookAround(double x, double y, double lastX, double lastY);
	void move(Camera::direction dir, int delta);
	void setAngle(Camera::angle angle, Vector3 punt, float distancia);
	void setPla(Camera::pla pla, Vector3 punt, float distancia);
	void display();

	bool isVisible(Vector3 pos, float marge);

	Camera(Vector3 pos, Vector3 target);
};
