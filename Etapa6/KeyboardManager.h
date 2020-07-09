#pragma once
#include <climits>
#include <vector>
#include <ctype.h>

//Classe que s'encarrega dels esdeveniments de teclat
class KeyboardManager {
private:
	static bool key[_UI8_MAX]; //True si la tecla est� pitjada

	// per controlar la invocaci� dels escoltadors de cada tecla
	static bool invoked[_UI8_MAX];
	static std::vector<void (*)()> keyHandlers[_UI8_MAX];
	static std::vector<void (*)(unsigned char)> genericHandlers;

	KeyboardManager() {}

public:

	//Funcions que cridar� GLUT quan es pitji o amolli una tecla
	static void onKeyUp(unsigned char key);
	static void onKeyDown(unsigned char key);

	//Comprova si una tecla est� pitjada
	static bool isPressed(unsigned char key);

	//Crida una funci� quan s'empra una tecla
	static void addKeyHandler(unsigned char key, void (*fn)());
	
	//Crida una funci� en pitjar qualsevol tecla
	static void addKeyHandler(void (*fn)(unsigned char key));
};

