#pragma once

template<class E>
class Vector2 
{

public:
	E x; //Components del vector
	E y;

	Vector2(E x, E y) {
		this->x = x;
		this->y = y;
	}

	~Vector2() {

	}

	E dot(Vector2 b) { //Producte
		return this->x * b.x + this->y * b.y;
	}
};