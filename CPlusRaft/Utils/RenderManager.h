#pragma once
#include <climits>
#include <vector>
#include <ctype.h>
#include <thread>
#include <algorithm>
#include <mutex>


//Gestió dels fils
class RenderManager {
private:
	static int cores; //True si la tecla està pitjada

	static std::vector<unsigned int> vaos;
	static std::vector<unsigned int> vbos;

	static std::mutex mutex;

	RenderManager() {}

public:

	static void removeVAO(unsigned int vao);
	static void removeVBO(unsigned int vbo);
	static void removeBuffers();

};

