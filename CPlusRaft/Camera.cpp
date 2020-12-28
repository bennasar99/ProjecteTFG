#include "Camera.h"

Camera::Camera(Vector3<float> pos, Vector3<float> target) {
	this->pos = pos;
	this->front = Vector3<float>::normalize(target - pos);
	this->updateVec();
	this->updateCorners();
}

//Actualitza els vectors de la càmera després de canviar front
void Camera::updateVec() {
	this->right = Vector3<float>::normalize(Vector3<float>::cross(this->worldUp, this->front));
	this->up = Vector3<float>::normalize(Vector3<float>::cross(this->front, this->right));
	this->camRight = Vector3<float>::normalize(Vector3<float>::cross(this->front, this->up));
}

Vector3<float> Camera::getPos() {
	return this->pos;
}

void Camera::setPos(Vector3<float> pos) {
	this->pos = pos;

	//Actualitzam la posició de l'escoltador OpenAL
	alListener3f(AL_POSITION, this->pos.x, this->pos.y, this->pos.z);
}

Vector3<float> Camera::getFront() {
	return this->front;
}

void Camera::setFront(Vector3<float> front) {
	this->front = front;
	this->updateVec();
	this->updateCorners();
}

Vector3<float> Camera::getRight() {
	return this->right;
}

void Camera::setRight(Vector3<float> right) {
	this->right = right;
	this->updateVec();
	this->updateCorners();
}

/*
	Estableix l'orientació de la càmera segons els moviments del ratolí
*/
void Camera::lookAround(double x, double y, double lastX, double lastY) {
	if (this->freeLook) {

		float xoffset = float(x - lastX);
		float yoffset = float(lastY - y);

		//Sensibilitat
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		//L'usuari no ha de poder rotar a l'axis Y
		if (pitch > 89.0f) {
			pitch = 89.0f;
		}
		else if (pitch < -89.0f) {
			pitch = -89.0f;
		}

		//Establim el nou vector front, segons pitch i yaw
		this->pitch = pitch;
		Vector3 direction = Vector3(cosf(toRad(yaw)) * cosf(toRad(pitch)), sinf(toRad(pitch)), sinf(toRad(yaw)) * cosf(toRad(pitch)));
		this->front = Vector3<float>::normalize(direction);
		this->updateVec(); //Actualitzam la resta de vectors

		//Establim l'orientació de l'escoltador OpenAL
		float orientation[6] = { this->front.x, this->front.y, this->front.z, this->up.x, this->up.y, this->up.z };
		alListenerfv(AL_ORIENTATION, orientation);

		this->updateCorners(); //Actualitzam els límits de l'àrea visible per la càmera
	}
}

//Mou la càmera en la direcció indicada
void Camera::move(Camera::direction dir, int delta) {
	if (this->freeMove) {
		float camSpeed = delta * this->speed;
		Vector3 front = this->front * camSpeed; front.y = 0;
		Vector3 right = this->camRight * camSpeed; right.y = 0;
		Vector3<float> up = Vector3<float>(0, 1.0f, 0) * camSpeed;
		switch (dir) {
		case direction::ENVANT:
			this->pos = this->pos + front;
			break;
		case direction::ENRERE:
			this->pos = this->pos - front;
			break;
		case direction::DRETA:
			this->pos = this->pos + right;
			break;
		case direction::ESQUERRA:
			this->pos = this->pos - right;
			break;
		case direction::ADALT:
			this->pos = this->pos + up;
			break;
		case direction::ABAIX:
			this->pos = this->pos - up;
			break;
		}
		//Actualitzam la posició de l'escoltador OpenAL
		alListener3f(AL_POSITION, this->pos.x, this->pos.y, this->pos.z);

		this->updateCorners();
	}
	if (this->drawMove) { //Guardam la nova posició de la càmera
		this->punts.push_front(this->pos);
		if (this->punts.size() > 1000) {
			this->punts.pop_back();
		}
	}
}

//Modifica la càmera perquè l'escena es vegi amb l'angle que correspongui
void Camera::setAngle(Camera::angle angle, Vector3<float> punt, float distancia) {
	switch (angle) {
	case angle::NADIR:
		this->pos = Vector3(punt.x, punt.y - distancia, punt.z);
		this->up = Vector3<float>(0, 0, 1);
		break;
	case angle::CONTRAPICAT:
		this->pos = Vector3(punt.x, punt.y - distancia, punt.z + distancia);
		this->up = Vector3<float>(0, 1, 0);
		break;
	case angle::NORMAL:
		this->pos = Vector3(punt.x, punt.y, punt.z + distancia);
		this->up = Vector3<float>(0, 1, 0);
		break;
	case angle::PICAT:
		this->pos = Vector3(punt.x, punt.y + distancia, punt.z + distancia);
		this->up = Vector3<float>(0, 1, 0);
		break;
	case angle::CENITAL:
		this->pos = Vector3(punt.x, punt.y + distancia, punt.z);
		this->up = Vector3<float>(0, 0, 1);
		break;
	}
	this->lookAt(punt);
}

//Modifica la càmera perquè l'escena es vegi amb el pla que correspongui
void Camera::setPla(Camera::pla pla, Vector3<float> punt, float distancia) {
	switch (pla) {
	case pla::PLANTA:
		this->pos = Vector3(punt.x, punt.y + distancia, punt.z);
		this->up = Vector3<float>(0, 0, 1);
		break;
	case pla::ALÇAT:
		this->pos = Vector3(punt.x - distancia, punt.y, punt.z);
		this->up = Vector3<float>(0, 1, 0);
		break;
	case pla::PERFIL:
		this->pos = Vector3(punt.x, punt.y, punt.z - distancia);
		this->up = Vector3<float>(0, 1, 0);
		break;
	case pla::PERSPECTIVA:
		this->pos = Vector3(punt.x + distancia, punt.y + distancia, punt.z + distancia);
		this->up = Vector3<float>(0, 1, 0);
		break;
	}
	this->lookAt(punt);
}

//Transforma la matriu de visualització perquè l'escena es vegi des dels ulls de la càmera
void Camera::display() {
	gluLookAt((double)this->pos.x, (double)this->pos.y, (double)this->pos.z,
		(double)this->pos.x + (double)this->front.x, (double)this->pos.y + (double)this->front.y, (double)this->pos.z + (double)this->front.z,
		(double)this->up.x, (double)this->up.y, (double)this->up.z);

	if (this->drawMove) {
		if (this->punts.size() > 0) {
			glPushMatrix();
			glDisable(GL_LIGHTING);
			std::list< Vector3<float> >::iterator it = this->punts.begin();
			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < (int)this->punts.size(); i++) {
				Vector3 p = *it;
				glVertex3f(p.x, p.y, p.z);
				it++;
			}
			glEnd();
			glEnable(GL_LIGHTING);
			glPopMatrix();
		}
	}
}

//Augmenta o disminuix el FOV segons calgui
void Camera::zoom(float zoom) {
	fov -= zoom;
	if (fov < 20.0f) {
		fov = 20.0f;
	}
	else if (fov > 90.0f) {
		fov = 90.0f;
	}
	this->updateCorners();
}

void Camera::setFov(float fov) {
	this->fov = fov;
}

float Camera::getFov() {
	return this->fov;
}

void Camera::setAspect(float aspect) {
	this->aspect = aspect;
}

float Camera::getAspect() {
	return this->aspect;
}

bool Camera::getFreeMove() {
	return this->freeMove;
}

void Camera::setFreeMove(bool set) {
	this->freeMove = set;
}

bool Camera::getFreeLook() {
	return this->freeLook;
}

void Camera::setFreeLook(bool set) {
	this->freeLook = set;
}

bool Camera::getDrawMove() {
	return this->drawMove;
}

void Camera::setDrawMove(bool set) {
	this->drawMove = set;
}
//Actualitzam 'estat de la càmera perque miri on s'indica

void Camera::lookAt(Vector3<float> pos) {
	this->front = Vector3<float>::normalize(pos - this->pos);
	this->pitch = toDegree(asinf(front.y));
	this->yaw = toDegree(atan2f(front.x, front.z));
	this->updateVec();
	this->updateCorners();
}

//Indica si un punt és visible per la càmera
bool Camera::isVisible(Vector3<float> pos, float marge) {
	float dist = Vector3<float>::module(pos - this->pos);
	if (dist < aspect) { //Abans 4 * aspect
		return true;
	}

	GLdouble model_view[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model_view);

	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport); //viewport[2] = width, viewport[3] = height

	double min = floor(marge*100/fov); //Abans marge*100
	double max = ceil(marge*100/fov);

	GLdouble winX, winY, winZ;
	gluProject(pos.x, pos.y, pos.z, model_view, projection, viewport, &winX, &winY, &winZ);
	if (-min <= winX && winX <= (viewport[2] + max) && -min <= winY && winY <= (viewport[3] + max) && winZ >= 0 && winZ <= 1) {
		return true;
	}
	else {
		return false;
	}
}

//Actualitza l'espai considerat visible per la càmera
void Camera::updateCorners() {
	Vector3 nearCenter = this->pos - this->front;
	Vector3 farCenter = this->pos + this->front * this->viewDist;

	float nearHeight = 2 * tanf(toRad(this->fov) / 2) * 1.0f;
	float farHeight = 2 * tanf(toRad(this->fov) / 2) * this->viewDist;
	float nearWidth = nearHeight * this->aspect;
	float farWidth = farHeight * this->aspect;

	Vector3 farTopLeft = farCenter + this->up * (farHeight * 0.5f) - this->camRight * (farWidth * 0.5f);
	Vector3 farTopRight = farCenter + this->up * (farHeight * 0.5f) + this->camRight * (farWidth * 0.5f);
	Vector3 farBottomLeft = farCenter - this->up * (farHeight * 0.5f) - this->camRight * (farWidth * 0.5f);
	Vector3 farBottomRight = farCenter - this->up * (farHeight * 0.5f) + this->camRight * (farWidth * 0.5f);

	Vector3 nearTopLeft = nearCenter + this->up * (nearHeight * 0.5f) - this->camRight * (nearWidth * 0.5f);
	Vector3 nearTopRight = nearCenter + this->up * (nearHeight * 0.5f) + this->camRight * (nearWidth * 0.5f);
	Vector3 nearBottomLeft = nearCenter - this->up * (nearHeight * 0.5f) - this->camRight * (nearWidth * 0.5f);
	Vector3 nearBottomRight = nearCenter - this->up * (nearHeight * 0.5f) + this->camRight * (nearWidth * 0.5f);

	xmin = (int)floor(std::min(std::min(std::min(farTopLeft.x, farTopRight.x), std::min(nearTopLeft.x, nearTopRight.x)), std::min(std::min(farBottomLeft.x, farBottomRight.x), std::min(nearBottomLeft.x, nearBottomRight.x))));
	xmax = (int)ceil(std::max(std::max(std::max(farTopLeft.x, farTopRight.x), std::max(nearTopLeft.x, nearTopRight.x)), std::max(std::max(farBottomLeft.x, farBottomRight.x), std::max(nearBottomLeft.x, nearBottomRight.x))));
	ymin = (int)floor(std::min(std::min(std::min(farTopLeft.y, farTopRight.y), std::min(nearTopLeft.y, nearTopRight.y)), std::min(std::min(farBottomLeft.y, farBottomRight.y), std::min(nearBottomLeft.y, nearBottomRight.y))));
	ymax = (int)ceil(std::max(std::max(std::max(farTopLeft.y, farTopRight.y), std::max(nearTopLeft.y, nearTopRight.y)), std::max(std::max(farBottomLeft.y, farBottomRight.y), std::max(nearBottomLeft.y, nearBottomRight.y))));
	zmin = (int)floor(std::min(std::min(std::min(farTopLeft.z, farTopRight.z), std::min(nearTopLeft.z, nearTopRight.z)), std::min(std::min(farBottomLeft.z, farBottomRight.z), std::min(nearBottomLeft.z, nearBottomRight.z))));
	zmax = (int)ceil(std::max(std::max(std::max(farTopLeft.z, farTopRight.z), std::max(nearTopLeft.z, nearTopRight.z)), std::max(std::max(farBottomLeft.z, farBottomRight.z), std::max(nearBottomLeft.z, nearBottomRight.z))));

}

void Camera::setViewDist(float viewDist) {
	this->viewDist = viewDist;
	updateCorners(); //La distància de visió ha canviat, actualitzam els límits de visibilitat
}

float Camera::getViewDist() {
	return this->viewDist;
}