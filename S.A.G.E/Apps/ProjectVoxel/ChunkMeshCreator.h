#pragma once

#include "TypeIds.h"

#include "ChunkData.h"
#include "ChunkRenderer.h"

class BlockData
{
public:
	BlockData(int index)
		:topBlockIndex(index), northSideBlockIndex(index), eastSideBlockIndex(index)
		, southSideBlockIndex(index), westSideBlockIndex(index), botBlockIndex(index)
	{
	}

	BlockData(int topIndex, int sideIndex, int botIndex)
		:topBlockIndex(topIndex), northSideBlockIndex(sideIndex), eastSideBlockIndex(sideIndex)
		, southSideBlockIndex(sideIndex), westSideBlockIndex(sideIndex), botBlockIndex(botIndex)
	{
	}

	BlockData(int topIndex, int northSideIndex, int eastSideIndex, int southSide, int westSide, int botIndex)
		:topBlockIndex(topIndex), northSideBlockIndex(northSideIndex), eastSideBlockIndex(eastSideIndex)
		, southSideBlockIndex(southSide), westSideBlockIndex(westSide), botBlockIndex(botIndex)
	{
	}

	~BlockData() {}

	int topBlockIndex = -1;
	int northSideBlockIndex = -1;
	int eastSideBlockIndex = -1;
	int southSideBlockIndex = -1;
	int westSideBlockIndex = -1;
	int botBlockIndex = -1;
};

class ChunkMeshCreator
{
public:
	ChunkMeshCreator();
	~ChunkMeshCreator();

	void CreateMeshFromData(const SAGE::Math::Vector2Int& chunkCoords, const BlockInfo data[18][32][18], ChunkRenderer& chunkRenderer);

private:
	void SkipCheckedBlocks(const SAGE::Math::RectInt& rect, int& right, int& up);

	std::unordered_map<BlockTypes, BlockData> mBlockData;
};