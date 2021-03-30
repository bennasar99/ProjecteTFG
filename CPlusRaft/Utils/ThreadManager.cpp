#include "ThreadManager.h"
#include "GL/glew.h"

int ThreadManager::cores;
std::vector<unsigned int> ThreadManager::vaos;
std::vector<unsigned int> ThreadManager::vbos;
std::mutex ThreadManager::mutex;

int ThreadManager::getCoreCount() {
	return ThreadManager::cores;
}

void ThreadManager::initialize() {

	//may return 0 when not able to detect
	ThreadManager::cores = std::thread::hardware_concurrency();
	cores = std::max(cores, 1);
	printf("Detectats %d fils\n", cores);
}
