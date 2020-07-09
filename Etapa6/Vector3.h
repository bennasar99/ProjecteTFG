#pragma once
#include "Math.h"
#include <stdio.h>

class Vector3
{
public:
	float x, y, z; //Components del vector

	Vector3();
	Vector3(float x, float y, float z);

	Vector3 operator+(const Vector3& b);
	Vector3 operator-(const Vector3& b);
	float operator*(const Vector3& b);
	Vector3 operator*(float b);

	static Vector3 cross(Vector3 a, Vector3 b);
	static float angle(Vector3 a, Vector3 b);
	static float module(Vector3 a);
	static Vector3 normalize(Vector3 a);

	bool isPositive();
	void noDecimals();
	bool equals(Vector3 other);
	void print();
};

