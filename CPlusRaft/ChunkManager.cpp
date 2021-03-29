#include "ChunkManager.h"

ChunkManager::ChunkManager() {

}


ChunkManager::~ChunkManager() {
	
}


void ChunkManager::setChunkState(Vector3<int> cPos, ChunkState cS) {
	std::unique_lock lock(cSMutex);
	cestat[cPos] = cS;
}

ChunkState ChunkManager::getChunkState(Vector3<int> cPos) {
	std::shared_lock lock(cSMutex);
	return cestat[cPos];
}

void ChunkManager::setRegionState(Vector3<int> rPos, RegionState rS) {
	std::unique_lock lock(rSMutex);
	restat[rPos] = rS;
}

RegionState ChunkManager::getRegionState(Vector3<int> rPos) {
	std::shared_lock lock(rSMutex);
	return restat[rPos];
}

void ChunkManager::removeChunk(Vector3<int> cPos) {
	{
		std::unique_lock lock(cSMutex);
		cestat.erase(cPos);
	}
	{
		//std::unique_lock lock(cMutex);
		//delete chunks[cPos];
		//chunks.erase(cPos);
	}
}

void ChunkManager::removeRegion(Vector3<int> rPos) {
	{
		std::unique_lock lock(rSMutex);
		restat.erase(rPos);
	}
}