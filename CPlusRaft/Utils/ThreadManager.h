#pragma once
#include <climits>
#include <vector>
#include <ctype.h>
#include <thread>
#include <algorithm>

//Classe que s'encarrega dels esdeveniments de teclat
class ThreadManager {
private:
	static int cores; //True si la tecla està pitjada

	ThreadManager() {}

public:

	static int getCoreCount();
	static void initialize();
};

