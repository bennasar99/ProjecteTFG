#pragma once
#include "Math.h"
#include <stdio.h>

template <class T = float>
class Vector3
{
public:
	T x, y, z; //Components del vector

	Vector3<T>();
	Vector3<T>(T x, T y, T z);

	template <class U>
	Vector3<T> operator+(const Vector3<U>& b);
	//Vector3<float> operator+(const Vector3<float>& b);
	Vector3 operator-(const Vector3<T>& b);
	float operator*(const Vector3<T>& b);
	Vector3 operator*(T b);
	Vector3 operator/(T b);
	Vector3 operator%(int b);
	bool operator!=(const Vector3<T>& b);

	static Vector3 cross(Vector3 a, Vector3 b);
	static float angle(Vector3 a, Vector3 b);
	static float module(Vector3 a);
	static Vector3 normalize(Vector3 a);

	bool isPositive();
	void noDecimals();
	bool equals(Vector3<T> other);
	void print();
	void floor();
};

template <class T>
inline Vector3<T>::Vector3() { //Constructor per defecte
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

template <class T>
inline Vector3<T>::Vector3(T x, T y, T z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

//Indica si tots els seus valors són positius
template <class T>
inline bool Vector3<T>::isPositive() {
	return (this->x >= 0 && this->y >= 0 && this->z >= 0);
}

//Compara 2 vectors
template <class T>
inline bool Vector3<T>::equals(Vector3<T> other) {
	return (this->x == other.x && this->y == other.y && this->z == other.z);
}

//Suma de vectors
//inline Vector3<float> Vector3<int>::operator+(const Vector3<float>& b) {
//	return Vector3<float>(this->x + b.x, this->y + b.y, this->z + b.z);
//}

template <class T>
template <class U>
inline Vector3<T> Vector3<T>::operator+(const Vector3<U>& b) {
	return Vector3(this->x + b.x, this->y + b.y, this->z + b.z);
}

//Resta de vectors
template <class T>
inline Vector3<T> Vector3<T>::operator-(const Vector3<T>& b) {
	return Vector3(this->x - b.x, this->y - b.y, this->z - b.z);
}

//Normalitza un vector
template <class T>
inline Vector3<T> Vector3<T>::normalize(Vector3<T> a) {
	float modul = Vector3::module(a);
	return Vector3(a.x / modul, a.y / modul, a.z / modul);
}

//Producte vectorial
template <class T>
inline Vector3<T> Vector3<T>::cross(Vector3<T> a, Vector3<T> b) {
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

//Producte d'un vector per un nombre
template <class T>
inline Vector3<T> Vector3<T>::operator*(T b) {
	return Vector3(this->x * b, this->y * b, this->z * b);
}

//Divisió d'un vector per un nombre
template <class T>
inline Vector3<T> Vector3<T>::operator/(T b) {
	return Vector3(this->x / b, this->y / b, this->z / b);
}

//Mòdul d'un vector per un nombre
template <class T>
inline Vector3<T> Vector3<T>::operator%(int b) {
	return Vector3((int)this->x % b, (int)this->y % b, (int)this->z % b);
}

template <class T>
inline bool Vector3<T>::operator!=(const Vector3<T>& b) {
	return(this->x != b.x || this->y != b.y || this->z != b.z);
}

//Producte escalar
template <class T>
inline float Vector3<T>::operator*(const Vector3<T>& b) {
	return this->x * b.x + this->y * b.y + this->z * b.z;
}

//Mòdul d'un vector
template <class T>
inline float Vector3<T>::module(Vector3<T> a) {
	return sqrtf(powf(a.x, 2.f) + powf(a.y, 2.f) + powf(a.z, 2.f));
}

//Angle entre 2 vectors
template <class T>
inline float Vector3<T>::angle(Vector3<T> a, Vector3<T> b) {
	float n1 = a * b;
	float modA = Vector3::module(a), modB = Vector3::module(b);
	float cosA = n1 / (modA * modB);
	return acosf(cosA);
}

//Arrodoneix les components del vector
inline void Vector3<float>::noDecimals() {
	this->x = roundf(this->x);
	this->y = roundf(this->y);
	this->z = roundf(this->z);
}

inline void Vector3<float>::floor() {
	this->x = floorf(this->x);
	this->y = floorf(this->y);
	this->z = floorf(this->z);
}

//Escriu l'informació del vector
inline void Vector3<float>::print() {
	printf("%f %f %f\n", this->x, this->y, this->z);
}

