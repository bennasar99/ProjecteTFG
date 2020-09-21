#pragma once
#include <climits>
#include <vector>
#include <ctype.h>

//Classe que s'encarrega dels esdeveniments de teclat
class KeyboardManager {
private:
	static bool key[512]; //True si la tecla està pitjada

	// per controlar la invocació dels escoltadors de cada tecla
	static bool invoked[512];
	static std::vector<void (*)()> keyHandlers[512];
	static std::vector<void (*)(int)> genericHandlers;

	KeyboardManager() {}

public:

	//Funcions que cridarà GLUT quan es pitji o amolli una tecla
	static void onKeyUp(int key);
	static void onKeyDown(int key);

	//Comprova si una tecla està pitjada
	static bool isPressed(int key);

	//Crida una funció quan s'empra una tecla
	static void addKeyHandler(int key, void (*fn)());
	
	//Crida una funció en pitjar qualsevol tecla
	static void addKeyHandler(void (*fn)(int key));
};

