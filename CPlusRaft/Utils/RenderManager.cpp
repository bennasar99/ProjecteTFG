#include "RenderManager.h"
#include "GL/glew.h"

std::vector<unsigned int> RenderManager::vaos;
std::vector<unsigned int> RenderManager::vbos;
std::mutex RenderManager::mutex;


void RenderManager::removeVAO(unsigned int vao) {
	const std::lock_guard<std::mutex> lock(mutex);
	RenderManager::vaos.push_back(vao);
}

void RenderManager::removeVBO(unsigned int vbo) {
	const std::lock_guard<std::mutex> lock(mutex);
	RenderManager::vbos.push_back(vbo);
}

void RenderManager::removeBuffers() {
	const std::lock_guard<std::mutex> lock(mutex);
	std::vector<unsigned int>::iterator it;
	for (it = RenderManager::vbos.begin(); it != RenderManager::vbos.end(); it++) {
		glDeleteBuffers(1, &(*it));
	}
	vbos.clear();
	for (it = RenderManager::vaos.begin(); it != RenderManager::vaos.end(); it++) {
		glDeleteVertexArrays(1, &(*it));
	}
	vaos.clear();
}
