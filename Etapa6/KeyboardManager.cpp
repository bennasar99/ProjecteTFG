#include "KeyboardManager.h"

bool KeyboardManager::key[512];
bool KeyboardManager::invoked[512]; //False

std::vector<void (*)()> KeyboardManager::keyHandlers[512];
std::vector<void (*)(int)> KeyboardManager::genericHandlers;

bool KeyboardManager::isPressed(int key) {
	return KeyboardManager::key[key];
}

void KeyboardManager::onKeyDown(int key) {
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

void KeyboardManager::onKeyUp(int key) {
	// marcar que es pot tornar a cridar l'esdeveniment de la tecla
	KeyboardManager::invoked[key] = false;
	KeyboardManager::key[key] = false;
}

/**
  * Afegeix un escoltador a una determinada tecla
  */
void KeyboardManager::addKeyHandler(int key, void (*fn)()) {
	KeyboardManager::keyHandlers[key].push_back(fn);
}


void KeyboardManager::addKeyHandler(void (*fn)(int key)) {
	KeyboardManager::genericHandlers.push_back(fn);
}
