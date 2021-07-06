#pragma once
#include <climits>
#include <vector>
#include <ctype.h>
#include <thread>
#include <algorithm>
#include <mutex>

//Serveix per eliminar aquells VAOs i VBOs que deixen de ser �tils per acci� del fil d'actualitzaci�. 
//El fil de rendertizaci� els ha d'eliminar m�s tard.
class RenderManager {
private:

	static std::vector<unsigned int> vaos;
	static std::vector<unsigned int> vbos;

	static std::mutex mutex;

	RenderManager() {}

public:

	static void removeVAO(unsigned int vao);
	static void removeVBO(unsigned int vbo);
	static void removeBuffers();

};

