#pragma once
#include <climits>
#include <vector>
#include <ctype.h>
#include <thread>
#include <algorithm>
#include <mutex>

//Gestió dels fils
class ThreadManager {
private:
	static int cores; //True si la tecla està pitjada

	static std::vector<unsigned int> vaos;
	static std::vector<unsigned int> vbos;

	static std::mutex mutex;

	ThreadManager() {}

public:

	static int getCoreCount();
	static void initialize();

	static void removeVAO(unsigned int vao);
	static void removeVBO(unsigned int vbo);
	static void removeBuffers();
};

