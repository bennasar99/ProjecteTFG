#pragma once

#include "Utils/Vector3.h"
#include <list>
#include "Utils.h"
#include <algorithm>
#include <al.h>

class Menu {
private:
	bool active;

public:
	
	void display();

	Menu();

	bool getActive();
	void setActive(bool active);
};
