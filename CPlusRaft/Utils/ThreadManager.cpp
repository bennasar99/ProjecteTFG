#include "ThreadManager.h"

int ThreadManager::cores;

int ThreadManager::getCoreCount() {
	return ThreadManager::cores;
}

void ThreadManager::initialize() {

	//may return 0 when not able to detect
	ThreadManager::cores = std::thread::hardware_concurrency();
	cores = std::max(cores, 1);
	printf("Detectats %d fils\n", cores);
}