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

void ThreadManager::removeVAO(unsigned int vao) {
	const std::lock_guard<std::mutex> lock(mutex);
	ThreadManager::vaos.push_back(vao);
}

void ThreadManager::removeVBO(unsigned int vbo) {
	const std::lock_guard<std::mutex> lock(mutex);
	ThreadManager::vaos.push_back(vbo);
}

void ThreadManager::removeBuffers() {
	const std::lock_guard<std::mutex> lock(mutex);
	std::vector<unsigned int>::iterator it;
	for (it = ThreadManager::vbos.begin(); it != ThreadManager::vbos.begin(); it++) {
		glDeleteBuffers(1, &(*it));
	}
	for (it = ThreadManager::vaos.begin(); it != ThreadManager::vaos.begin(); it++) {
		//glDeleteBuffers(1, &(*it));
	}
}