#include "KeyboardManager.h"

bool KeyboardManager::key[_UI8_MAX];
bool KeyboardManager::invoked[_UI8_MAX]; //False

std::vector<void (*)()> KeyboardManager::keyHandlers[_UI8_MAX];
std::vector<void (*)(unsigned char)> KeyboardManager::genericHandlers;

bool KeyboardManager::isPressed(unsigned char key) {
	return KeyboardManager::key[tolower(key)];
}

void KeyboardManager::onKeyDown(unsigned char key) {
	// invocar tots els escoltadors d'aquesta tecla (si no s'han invocat)
	if (!KeyboardManager::invoked[key]) {
		for (size_t i = 0; i < KeyboardManager::keyHandlers[key].size(); ++i) {
			KeyboardManager::keyHandlers[key][i]();
		}

		for (size_t i = 0; i < KeyboardManager::genericHandlers.size(); ++i) {
			KeyboardManager::genericHandlers[i](key);
		}

		KeyboardManager::invoked[key] = true;
	}

	KeyboardManager::key[key] = true;
}

void KeyboardManager::onKeyUp(unsigned char key) {
	// marcar que es pot tornar a cridar l'esdeveniment de la tecla
	KeyboardManager::invoked[key] = false;
	KeyboardManager::key[key] = false;
}

/**
  * Afegeix un escoltador a una determinada tecla
  */
void KeyboardManager::addKeyHandler(unsigned char key, void (*fn)()) {
	KeyboardManager::keyHandlers[key].push_back(fn);
}


void KeyboardManager::addKeyHandler(void (*fn)(unsigned char k)) {
	KeyboardManager::genericHandlers.push_back(fn);
}
