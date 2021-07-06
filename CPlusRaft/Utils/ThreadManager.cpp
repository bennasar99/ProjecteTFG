#include "ThreadManager.h"
#include "GL/glew.h"

int ThreadManager::cores;
std::mutex ThreadManager::mutex;

int ThreadManager::getCoreCount() {
	return ThreadManager::cores;
}

void ThreadManager::initialize() {

	ThreadManager::cores = std::thread::hardware_concurrency();
	cores = std::max(cores, 1);
	printf("Detectats %d fils\n", cores);
}
