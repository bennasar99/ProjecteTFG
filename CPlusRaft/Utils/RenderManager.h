#pragma once
#include <climits>
#include <vector>
#include <ctype.h>
#include <thread>
#include <algorithm>
#include <mutex>

//Serveix per eliminar aquells VAOs i VBOs que deixen de ser útils per acció del fil d'actualització. 
//El fil de rendertizació els ha d'eliminar més tard.
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

