#pragma once
#include <climits>
#include <vector>
#include <ctype.h>

#define KEYNUM 512

//Classe que s'encarrega dels esdeveniments de teclat
class KeyboardManager {
private:
	static bool key[KEYNUM]; //True si la tecla està pitjada

	// per controlar la invocació dels escoltadors de cada tecla
	static bool invoked[KEYNUM];
	static std::vector<void (*)()> keyHandlers[KEYNUM];
	static std::vector<void (*)(int)> genericHandlers;

	KeyboardManager() {}

public:

	//Funcions que cridarà GLFW quan l'usuari interactui amb el teclat
	static void onKeyUp(int key);
	static void onKeyDown(int key);

	//Comprova si una tecla està pitjada
	static bool isPressed(int key);

	//Crida una funció quan s'empra una tecla
	static void addKeyHandler(int key, void (*fn)());
	
	//Crida una funció en pitjar qualsevol tecla
	static void addKeyHandler(void (*fn)(int key));
};

