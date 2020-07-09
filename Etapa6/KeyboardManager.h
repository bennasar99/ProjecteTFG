#pragma once
#include <climits>
#include <vector>
#include <ctype.h>

//Classe que s'encarrega dels esdeveniments de teclat
class KeyboardManager {
private:
	static bool key[_UI8_MAX]; //True si la tecla està pitjada

	// per controlar la invocació dels escoltadors de cada tecla
	static bool invoked[_UI8_MAX];
	static std::vector<void (*)()> keyHandlers[_UI8_MAX];
	static std::vector<void (*)(unsigned char)> genericHandlers;

	KeyboardManager() {}

public:

	//Funcions que cridarà GLUT quan es pitji o amolli una tecla
	static void onKeyUp(unsigned char key);
	static void onKeyDown(unsigned char key);

	//Comprova si una tecla està pitjada
	static bool isPressed(unsigned char key);

	//Crida una funció quan s'empra una tecla
	static void addKeyHandler(unsigned char key, void (*fn)());
	
	//Crida una funció en pitjar qualsevol tecla
	static void addKeyHandler(void (*fn)(unsigned char key));
};

