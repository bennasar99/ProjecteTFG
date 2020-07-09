#include "Vector3.h"

Vector3::Vector3() { //Constructor per defecte
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

//Indica si tots els seus valors són positius
bool Vector3::isPositive() {
	return (this->x >= 0 && this->y >= 0 && this->z >= 0);
}

//Compara 2 vectors
bool Vector3::equals(Vector3 other) {
	return (this->x == other.x && this->y == other.y && this->z == other.z);
}

//Suma de vectors
Vector3 Vector3::operator+(const Vector3& b) {
	return Vector3(this->x + b.x, this->y + b.y, this->z + b.z);
}

//Resta de vectors
Vector3 Vector3::operator-(const Vector3& b) {
	return Vector3(this->x - b.x, this->y - b.y, this->z - b.z);
}

//Normalitza un vector
Vector3 Vector3::normalize(Vector3 a) { 
	float modul = Vector3::module(a);
	return Vector3(a.x / modul, a.y / modul, a.z / modul);
}

//Producte vectorial
Vector3 Vector3::cross(Vector3 a, Vector3 b) {
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

//Producte d'un vector per un nombre
Vector3 Vector3::operator*(float b) {
	return Vector3(this->x * b, this->y * b, this->z * b);
}

//Producte escalar
float Vector3::operator*(const Vector3& b) {
	return this->x * b.x + this->y * b.y + this->z * b.z;
}

//Mòdul d'un vector
float Vector3::module(Vector3 a) {
	return sqrtf(powf(a.x, 2.f) + powf(a.y, 2.f) + powf(a.z, 2.f));
}

//Angle entre 2 vectors
float Vector3::angle(Vector3 a, Vector3 b) {
	float n1 = a * b;
	float modA = Vector3::module(a), modB = Vector3::module(b);
	float cosA = n1 / (modA * modB);
	return acosf(cosA);
}

//Arrodoneix les components del vector
void Vector3::noDecimals() {
	this->x = roundf(this->x);
	this->y = roundf(this->y);
	this->z = roundf(this->z);
}

//Escriu l'informació del vector
void Vector3::print() {
	printf("%f %f %f\n", this->x, this->y, this->z);
}