#include "ChunkStateManager.h"

ChunkStateManager::ChunkStateManager() {

}


ChunkStateManager::~ChunkStateManager() {
	
}


void ChunkStateManager::setChunkState(Vector3<int> cPos, ChunkState cS) {
	std::unique_lock lock(cSMutex);
	cestat[cPos] = cS;
}

ChunkState ChunkStateManager::getChunkState(Vector3<int> cPos) {
	std::shared_lock lock(cSMutex);
	std::unordered_map<Vector3<int>, ChunkState>::const_iterator iter = cestat.find(cPos);

	if (iter == cestat.end())
	{
		return ChunkState::BUIT;
	}
	return iter->second;
}

void ChunkStateManager::setRegionState(Vector3<int> rPos, RegionState rS) {
	std::unique_lock lock(rSMutex);
	restat[rPos] = rS;
}

RegionState ChunkStateManager::getRegionState(Vector3<int> rPos) {
	std::shared_lock lock(rSMutex);

	std::unordered_map<Vector3<int>, RegionState>::const_iterator iter = restat.find(rPos);

	if (iter == restat.end())
	{
		return RegionState::BUIDA;
	}
	return iter->second;
}

void ChunkStateManager::removeChunk(Vector3<int> cPos) {
	{
		std::unique_lock lock(cSMutex);
		cestat.erase(cPos);
	}
}

void ChunkStateManager::removeRegion(Vector3<int> rPos) {
	{
		std::unique_lock lock(rSMutex);
		restat.erase(rPos);
	}
}