#pragma once

#include "Blocks/Block.h"
#include "Render/ChunkMesh.h"
#include "Utils/Vector3.h"
#include "Chunk.h"
#include "Generation/WorldGenerator.h"
#include <list>
#include <shared_mutex>
#include <map>

enum class ChunkState : uint8_t {
	BUIT,
	LLEST,
	TERRENY,
	PENDENT,
	PENDENT2,
	CARREGAT,
};

enum class RegionState : uint8_t {
	BUIDA,
	PENDENT,
	LLEST,
	DIRTY,
};

class ChunkManager {
private:
	//Block* blocs[CHUNKSIZE][CHUNKSIZE][CHUNKSIZE] = {};
	//int height[CHUNKSIZE][CHUNKSIZE]; //Heightmap
	std::map<Vector3<int>, ChunkState> cestat;
	std::map<Vector3<int>, RegionState> restat;
	std::map < Vector3<int>, std::array<std::array<int, CHUNKSIZE>, CHUNKSIZE > > height;
	/*map<Vector3<int>, Chunk*> chunks;*/

	std::shared_mutex cSMutex;
	std::shared_mutex rSMutex;

	std::shared_mutex cMutex;

public:

	void setChunkState(Vector3<int> cPos, ChunkState cS);
	ChunkState getChunkState(Vector3<int> cPos);

	void setRegionState(Vector3<int> rPos, RegionState rS);
	RegionState getRegionState(Vector3<int> rPos);

	void removeChunk(Vector3<int> cPos);
	void removeRegion(Vector3<int> rPos);

	ChunkManager();
	~ChunkManager();
};