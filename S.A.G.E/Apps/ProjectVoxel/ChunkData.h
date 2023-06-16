#pragma once

#include <SAGE/Inc/SAGE.h>
#include "Enums.h"

struct BlockInfo
{
	BlockInfo()
	{
	}

	BlockInfo(BlockTypes bt, DirectionVector dv, bool floored, int8_t padding)
		:blockTypes(bt), compass(dv), isFloored(floored), padding(padding)
	{
	}

	BlockInfo(BlockTypes bt, DirectionVector dv, int8_t padding)
		:blockTypes(bt), compass(dv), padding(padding)
	{
	}

	BlockInfo(BlockTypes bt, bool floored, int8_t padding)
		:blockTypes(bt), isFloored(floored), padding(padding)
	{
	}

	BlockInfo(BlockTypes bt, int8_t padding)
		:blockTypes(bt), padding(padding)
	{
	}

	BlockInfo(BlockTypes bt)
		:blockTypes(bt)
	{
	}

	bool operator==(const BlockInfo& rhs)
	{
		return blockTypes == rhs.blockTypes && compass == rhs.compass && isFloored == rhs.isFloored && padding == rhs.padding;
	}

	BlockTypes blockTypes = BlockTypes::Air;
	DirectionVector compass = DirectionVector::North;
	bool isFloored = true;
	int8_t padding = 0;
};

class ChunkData
{
public:
	ChunkData(std::unordered_map<std::string, ChunkData>* chunkData, SAGE::Math::Vector2Int chunkCoords)
		:chunkDataPtr(chunkData), chunkCoords(chunkCoords)
	{
	}
	~ChunkData()
	{
		chunkDataPtr = nullptr;
	}

	SAGE::Math::Vector3Int GetBlock(SAGE::Math::Vector2Int chunkCoords, SAGE::Math::Vector3 worldPos)
	{
		bool isXNegative = false;
		bool isZNegative = false;

		// Get Local Point
		if (chunkCoords.x < 0) {
			chunkCoords.x *= -1;
			worldPos.x *= -1.0f;
			isXNegative = true;
		}
		if (chunkCoords.y < 0) {
			chunkCoords.y *= -1;
			worldPos.z *= -1.0f;
			isZNegative = true;
		}

		for (int x = chunkCoords.x; x > 0; --x) {
			worldPos.x -= 16.0f;
		}

		for (int z = chunkCoords.y; z > 0; --z) {
			worldPos.z -= 16.0f;
		}
		worldPos.x += 8.0f;
		worldPos.z += 8.0f;

		int blockX = isXNegative ? (static_cast<int>(worldPos.x) - 15) * -1 : static_cast<int>(worldPos.x);
		const int blockY = static_cast<int>(worldPos.y);
		int blockZ = isZNegative ? (static_cast<int>(worldPos.z) - 15) * -1 : static_cast<int>(worldPos.z);

		return { blockX, blockY, blockZ };
	}

	BlockInfo& GetBlockInfo(SAGE::Math::Vector3Int blockCoords) { 
		SAGE::Math::Vector2Int tempChunkCoords = GetChunkCoords(blockCoords);
		if (tempChunkCoords != chunkCoords)
		{
			auto& chunkData = chunkDataPtr->at(KeyGenerator(tempChunkCoords));
			return chunkData.blocksData[blockCoords.x][blockCoords.y][blockCoords.z];
		}
		return blocksData[blockCoords.x][blockCoords.y][blockCoords.z];
	}

	bool SetBlock(SAGE::Math::Vector3Int block, BlockInfo blockInfo)
	{
		SAGE::Math::Vector2Int tempChunkCoords = GetChunkCoords(block);
		if (tempChunkCoords != chunkCoords)
		{
			auto& chunkData = chunkDataPtr->at(KeyGenerator(tempChunkCoords));
			return chunkData.SetBlock(block, blockInfo);
		}

		if (blocksData[block.x][block.y][block.z] == blockInfo) {
			return false;
		}

		blocksData[block.x][block.y][block.z] = blockInfo;
		return true;
	}

	SAGE::Math::Vector2Int GetChunkCoords(SAGE::Math::Vector3Int& blockCoords)
	{
		SAGE::Math::Vector2Int tempChunkCoords = chunkCoords;

		while (blockCoords.x > 15)
		{
			tempChunkCoords.x += 1;
			blockCoords.x -= 16;
		}

		while (blockCoords.x < 0)
		{
			tempChunkCoords.x -= 1;
			blockCoords.x += 16;
		}

		while (blockCoords.z > 15)
		{
			tempChunkCoords.y += 1;
			blockCoords.z -= 16;
		}

		while (blockCoords.z < 0)
		{
			tempChunkCoords.y -= 1;
			blockCoords.z += 16;
		}

		return tempChunkCoords;
	}

	BiomeTypes biomeType = BiomeTypes::Plains;
	SAGE::Math::Vector2Int chunkCoords = SAGE::Math::Vector2Int::Zero;
	BlockInfo blocksData[16][32][16];

private:
	const std::string KeyGenerator(const SAGE::Math::Vector2Int& coords)
	{
		return std::to_string(coords.x) + ", " + std::to_string(coords.y);
	}

	std::unordered_map<std::string, ChunkData>* chunkDataPtr;

};