#include "ChunkMeshCreator.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

namespace
{
	bool IsBlockTransparent(const BlockTypes block)
	{
		switch (block)
		{
		case BlockTypes::Air:
		case BlockTypes::OakLeaves:
		case BlockTypes::Grass:
		case BlockTypes::Poppy:
		case BlockTypes::Dandelion:
		case BlockTypes::SugarCane:
		case BlockTypes::Cactus:
		case BlockTypes::Glass:
		case BlockTypes::Water:
		case BlockTypes::Wheat:
			return true;
		default: return false;
		}
	}

	bool IsVegetative(const BlockTypes block)
	{
		switch (block)
		{
		case BlockTypes::Grass:
		case BlockTypes::Poppy:
		case BlockTypes::Dandelion:
		case BlockTypes::SugarCane:
		case BlockTypes::Wheat:
			return true;
		default: return false;
		}
	}

	bool IsSlab(const BlockTypes block)
	{
		switch (block)
		{
		case BlockTypes::OakSlab:
		case BlockTypes::CobbleStoneSlab:
		case BlockTypes::StoneSlab:
			return true;
		default: return false;
		}
	}

	bool IsFarmable(const BlockTypes block)
	{
		switch (block)
		{
		case BlockTypes::Wheat:
			return true;
		default: return false;
		}
	}
}

ChunkMeshCreator::ChunkMeshCreator()
{
	mBlockData.insert(std::make_pair(BlockTypes::BedRock, BlockData(0)));
	mBlockData.insert(std::make_pair(BlockTypes::GrassBlock, BlockData(1, 2, 3)));
	mBlockData.insert(std::make_pair(BlockTypes::Dirt, BlockData(3)));
	mBlockData.insert(std::make_pair(BlockTypes::CobbleStone, BlockData(4)));
	mBlockData.insert(std::make_pair(BlockTypes::Stone, BlockData(5)));
	mBlockData.insert(std::make_pair(BlockTypes::OakLeaves, BlockData(6)));
	mBlockData.insert(std::make_pair(BlockTypes::OakLog, BlockData(7, 8, 7)));
	mBlockData.insert(std::make_pair(BlockTypes::OakPlank, BlockData(9)));

	mBlockData.insert(std::make_pair(BlockTypes::Glass, BlockData(10)));
	mBlockData.insert(std::make_pair(BlockTypes::Sand, BlockData(11)));
	mBlockData.insert(std::make_pair(BlockTypes::Gravel, BlockData(12)));
	mBlockData.insert(std::make_pair(BlockTypes::CoalOre, BlockData(13)));
	mBlockData.insert(std::make_pair(BlockTypes::IronOre, BlockData(14)));
	mBlockData.insert(std::make_pair(BlockTypes::GoldOre, BlockData(15)));
	mBlockData.insert(std::make_pair(BlockTypes::DiamondOre, BlockData(16)));
	mBlockData.insert(std::make_pair(BlockTypes::RedStoneOre, BlockData(17)));
	mBlockData.insert(std::make_pair(BlockTypes::LapisOre, BlockData(18)));
	mBlockData.insert(std::make_pair(BlockTypes::EmeraldOre, BlockData(19)));
	mBlockData.insert(std::make_pair(BlockTypes::SnowGrassBlock, BlockData(21, 20, 3)));
	mBlockData.insert(std::make_pair(BlockTypes::Snow, BlockData(21)));
	mBlockData.insert(std::make_pair(BlockTypes::Cactus, BlockData(22, 23, 22)));

	mBlockData.insert(std::make_pair(BlockTypes::CraftingTable, BlockData(28, 29, 30, 29, 30, 28)));
	mBlockData.insert(std::make_pair(BlockTypes::Furnace, BlockData(31, 33, 33, 32, 33, 31)));

	mBlockData.insert(std::make_pair(BlockTypes::OakSlab, BlockData(9)));
	mBlockData.insert(std::make_pair(BlockTypes::CobbleStoneSlab, BlockData(4)));
	mBlockData.insert(std::make_pair(BlockTypes::StoneSlab, BlockData(5)));

	mBlockData.insert(std::make_pair(BlockTypes::Grass, BlockData(24)));
	mBlockData.insert(std::make_pair(BlockTypes::Poppy, BlockData(25)));
	mBlockData.insert(std::make_pair(BlockTypes::Dandelion, BlockData(26)));
	mBlockData.insert(std::make_pair(BlockTypes::SugarCane, BlockData(27)));

	mBlockData.insert(std::make_pair(BlockTypes::Lava, BlockData(-1)));
	mBlockData.insert(std::make_pair(BlockTypes::Water, BlockData(-1)));
	mBlockData.insert(std::make_pair(BlockTypes::Companion, BlockData(-1)));

	mBlockData.insert(std::make_pair(BlockTypes::Farmland, BlockData(34, 3, 3)));
	mBlockData.insert(std::make_pair(BlockTypes::Wheat, BlockData(35)));
}

ChunkMeshCreator::~ChunkMeshCreator()
{
	mBlockData.clear();
}

void ChunkMeshCreator::CreateMeshFromData(const SAGE::Math::Vector2Int& chunkCoords, const BlockInfo data[18][32][18], ChunkRenderer& chunkRenderer)
{
	chunkRenderer.activeBlocks.clear();
	chunkRenderer.activeBlocksIndexs.clear();

	VoxelMesh opaqueMesh;
	VoxelMesh transMesh;
	VoxelMesh waterMesh;
	const float size = 0.5f;
	const float offsetX = chunkCoords.x * 16 - 9 + size; // -9 = -8 - 1 (for offset)
	const float offsetZ = chunkCoords.y * 16 - 9 + size; // -9 = -8 - 1 (for offset)

	bool isYEmpty[32] = { true };

	for (int x = 1; x < 17; ++x)
	{
		for (int z = 1; z < 17; ++z)
		{
			for (int y = 0; y < 32; ++y)
			{
				if (data[x][y][z].blockTypes == BlockTypes::Air) { continue; }
				bool addedBlock = false;
				const auto& blockData = mBlockData.at(data[x][y][z].blockTypes);

				if (!IsBlockTransparent(data[x][y][z].blockTypes))
				{
					//Front
					if (IsBlockTransparent(data[x][y][z + 1].blockTypes) || IsSlab(data[x][y][z + 1].blockTypes))
					{
						addedBlock = true;
					}
					//Back
					else if (IsBlockTransparent(data[x][y][z - 1].blockTypes) || IsSlab(data[x][y][z - 1].blockTypes))// == BlockTypes::Air)
					{
						addedBlock = true;
					}
					//Left
					else if (IsBlockTransparent(data[x + 1][y][z].blockTypes) || IsSlab(data[x + 1][y][z].blockTypes))// == BlockTypes::Air)
					{
						addedBlock = true;
					}
					//Right		
					else if (IsBlockTransparent(data[x - 1][y][z].blockTypes) || IsSlab(data[x - 1][y][z].blockTypes))// == BlockTypes::Air)
					{
						addedBlock = true;
					}
					//Top		
					else if (y == 31 || IsBlockTransparent(data[x][y + 1][z].blockTypes) || IsSlab(data[x][y][z].blockTypes))// == BlockTypes::Air)
					{
						addedBlock = true;
					}
					//Bot	
					else if (y < 31 && y > 0 && (IsBlockTransparent(data[x][y - 1][z].blockTypes) || (IsSlab(data[x][y][z].blockTypes) && IsSlab(data[x][y - 1][z].blockTypes))))
					{
						addedBlock = true;
					}

					if (addedBlock)
					{
						isYEmpty[y] = false;
						chunkRenderer.activeBlocks.push_back({ x + offsetX, y + size, z + offsetZ });
						chunkRenderer.activeBlocksIndexs.push_back({ x - 1, y, z - 1 });
					}
				}
				else
				{
					if (IsVegetative(data[x][y][z].blockTypes))
					{
						const auto blockData = mBlockData.at(data[x][y][z].blockTypes);

						const float minX = x - size + offsetX;
						const float maxX = x + size + offsetX;
						const float minY = static_cast<float>(y);
						const float maxY = y + size + size;
						const float minZ = z - size + offsetZ;
						const float maxZ = z + size + offsetZ;

						const int blockIndex = blockData.northSideBlockIndex + (IsFarmable(data[x][y][z].blockTypes) ? static_cast<int>(data[x][y][z].padding) : 0);

						//Top Left to Bot Right
						transMesh.vertices.push_back({ {minX, minY, maxZ}, Vector3::ZAxis, Vector3::XAxis, {1.0f, 1.0f}, blockIndex });		//0 - FrontBotLeft
						transMesh.vertices.push_back({ {minX, maxY, maxZ}, Vector3::ZAxis, Vector3::XAxis, {1.0f, 0.0f}, blockIndex });		//1 - FrontTopLeft
						transMesh.vertices.push_back({ {maxX, maxY, minZ}, -Vector3::ZAxis, Vector3::XAxis, {0.0f, 0.0f}, blockIndex });	//2 - BackTopRight
						transMesh.vertices.push_back({ {maxX, minY, minZ}, -Vector3::ZAxis, Vector3::XAxis, {0.0f, 1.0f}, blockIndex });	//3 - BackBotRight

						int count = static_cast<int>(transMesh.vertices.size()) - (transMesh.vertices.size() == 0 ? 0 : 4);

						transMesh.indices.push_back(2 + count);
						transMesh.indices.push_back(1 + count);
						transMesh.indices.push_back(0 + count);

						transMesh.indices.push_back(2 + count);
						transMesh.indices.push_back(0 + count);
						transMesh.indices.push_back(3 + count);

						transMesh.indices.push_back(0 + count);
						transMesh.indices.push_back(1 + count);
						transMesh.indices.push_back(2 + count);

						transMesh.indices.push_back(3 + count);
						transMesh.indices.push_back(0 + count);
						transMesh.indices.push_back(2 + count);

						//Top Right to Bot Left
						transMesh.vertices.push_back({ {maxX, maxY, maxZ}, Vector3::ZAxis, Vector3::XAxis, {1.0f, 0.0f}, blockIndex });		//0 - FrontTopRight
						transMesh.vertices.push_back({ {maxX, minY, maxZ}, Vector3::ZAxis, Vector3::XAxis, {1.0f, 1.0f}, blockIndex });		//1 - FrontBotRight
						transMesh.vertices.push_back({ {minX, minY, minZ}, -Vector3::ZAxis, Vector3::XAxis, {0.0f, 1.0f}, blockIndex });	//2 - BackBotLeft
						transMesh.vertices.push_back({ {minX, maxY, minZ}, -Vector3::ZAxis, Vector3::XAxis, {0.0f, 0.0f}, blockIndex });	//3 - BackTopLeft

						count = static_cast<int>(transMesh.vertices.size()) - (transMesh.vertices.size() == 0 ? 0 : 4);

						transMesh.indices.push_back(2 + count);
						transMesh.indices.push_back(1 + count);
						transMesh.indices.push_back(0 + count);

						transMesh.indices.push_back(2 + count);
						transMesh.indices.push_back(0 + count);
						transMesh.indices.push_back(3 + count);

						transMesh.indices.push_back(0 + count);
						transMesh.indices.push_back(1 + count);
						transMesh.indices.push_back(2 + count);

						transMesh.indices.push_back(3 + count);
						transMesh.indices.push_back(0 + count);
						transMesh.indices.push_back(2 + count);

						chunkRenderer.activeBlocks.push_back({ x + offsetX, y + size, z + offsetZ });
						chunkRenderer.activeBlocksIndexs.push_back({ x - 1, y, z - 1 });
					}
					else if (data[x][y][z].blockTypes == BlockTypes::Water)
					{
						const float waterOffsetY = -0.1f;
						const float minY = static_cast<float>(y);
						const float maxY = data[x][y + 1][z].blockTypes == BlockTypes::Water ? y + size + size : Lerp(minY, y + size + size + waterOffsetY, static_cast<float>(data[x][y][z].padding) / 7.0f);
						const int botMove = data[x][y - 1][z].blockTypes == BlockTypes::Water ? 1 : 0;
						//const int moveWater = y <= 30 ? (data[x][y + 1][z].blockTypes == BlockTypes::Water ? 0 : 1) : (1);
						const int moveWater = 1;
						float botLeftMaxY = maxY;
						float botRightMaxY = maxY;
						float topLeftMaxY = maxY;
						float topRightMaxY = maxY;

						if (data[x][y + 1][z].blockTypes != BlockTypes::Water)
						{
							int8_t botLeftMaxYPadding = data[x][y][z].padding;
							if (data[x - 1][y][z].blockTypes == BlockTypes::Water)
							{
								botLeftMaxYPadding = Max(botLeftMaxYPadding, data[x - 1][y][z].padding);
							}
							if (data[x - 1][y][z - 1].blockTypes == BlockTypes::Water)
							{
								botLeftMaxYPadding = Max(botLeftMaxYPadding, data[x - 1][y][z - 1].padding);
							}
							if (data[x][y][z - 1].blockTypes == BlockTypes::Water)
							{
								botLeftMaxYPadding = Max(botLeftMaxYPadding, data[x][y][z - 1].padding);
							}
							botLeftMaxY = Lerp(static_cast<float>(y), y + size + size + waterOffsetY, static_cast<float>(botLeftMaxYPadding) / 7.0f);
							if (data[x - 1][y + 1][z - 1].blockTypes == BlockTypes::Water)
							{
								botLeftMaxY = y + size + size;
							}

							int8_t botRightMaxYPadding = data[x][y][z].padding;
							if (data[x + 1][y][z].blockTypes == BlockTypes::Water)
							{
								botRightMaxYPadding = Max(botRightMaxYPadding, data[x + 1][y][z].padding);
							}
							if (data[x + 1][y][z - 1].blockTypes == BlockTypes::Water)
							{
								botRightMaxYPadding = Max(botRightMaxYPadding, data[x + 1][y][z - 1].padding);
							}
							if (data[x][y][z - 1].blockTypes == BlockTypes::Water)
							{
								botRightMaxYPadding = Max(botRightMaxYPadding, data[x][y][z - 1].padding);
							}
							botRightMaxY = Lerp(static_cast<float>(y), y + size + size + waterOffsetY, static_cast<float>(botRightMaxYPadding) / 7.0f);
							if (data[x + 1][y + 1][z - 1].blockTypes == BlockTypes::Water)
							{
								botRightMaxY = y + size + size;
							}

							int8_t topLeftMaxYPadding = data[x][y][z].padding;
							if (data[x - 1][y][z].blockTypes == BlockTypes::Water)
							{
								topLeftMaxYPadding = Max(topLeftMaxYPadding, data[x - 1][y][z].padding);
							}
							if (data[x - 1][y][z + 1].blockTypes == BlockTypes::Water)
							{
								topLeftMaxYPadding = Max(topLeftMaxYPadding, data[x - 1][y][z + 1].padding);
							}
							if (data[x][y][z + 1].blockTypes == BlockTypes::Water)
							{
								topLeftMaxYPadding = Max(topLeftMaxYPadding, data[x][y][z + 1].padding);
							}
							topLeftMaxY = Lerp(static_cast<float>(y), y + size + size + waterOffsetY, static_cast<float>(topLeftMaxYPadding) / 7.0f);
							if (data[x - 1][y + 1][z + 1].blockTypes == BlockTypes::Water)
							{
								topLeftMaxY = y + size + size;
							}

							int8_t topRightMaxYPadding = data[x][y][z].padding;
							if (data[x + 1][y][z].blockTypes == BlockTypes::Water)
							{
								topRightMaxYPadding = Max(topRightMaxYPadding, data[x + 1][y][z].padding);
							}
							if (data[x + 1][y][z + 1].blockTypes == BlockTypes::Water)
							{
								topRightMaxYPadding = Max(topRightMaxYPadding, data[x + 1][y][z + 1].padding);
							}
							if (data[x][y][z + 1].blockTypes == BlockTypes::Water)
							{
								topRightMaxYPadding = Max(topRightMaxYPadding, data[x][y][z + 1].padding);
							}
							topRightMaxY = Lerp(static_cast<float>(y), y + size + size + waterOffsetY, static_cast<float>(topRightMaxYPadding) / 7.0f);
							if (data[x + 1][y + 1][z + 1].blockTypes == BlockTypes::Water)
							{
								topRightMaxY = y + size + size;
							}

							if (data[x][y + 1][z + 1].blockTypes == BlockTypes::Water)
							{
								topLeftMaxY = y + size + size;
								topRightMaxY = y + size + size;
							}

							if (data[x][y + 1][z - 1].blockTypes == BlockTypes::Water)
							{
								botLeftMaxY = y + size + size;
								botRightMaxY = y + size + size;
							}

							if (data[x + 1][y + 1][z].blockTypes == BlockTypes::Water)
							{
								topRightMaxY = y + size + size;
								botRightMaxY = y + size + size;
							}

							if (data[x - 1][y + 1][z].blockTypes == BlockTypes::Water)
							{
								topLeftMaxY = y + size + size;
								botLeftMaxY = y + size + size;
							}
						}

						// North
						//if (false)
						if (IsBlockTransparent(data[x][y][z + 1].blockTypes) && data[x][y][z + 1].blockTypes != BlockTypes::Water) // && data[x][y][z + 1].padding < 7
						{
							const float posZ = z + size + offsetZ;
							const float minX = x - size + offsetX;
							const float maxX = x + size + offsetX;

							waterMesh.vertices.push_back({ {maxX, minY, posZ}, Vector3::ZAxis, Vector3::XAxis, {0.0f, 1.0f}, botMove }); // 0 Bot Left
							waterMesh.vertices.push_back({ {minX, minY, posZ}, Vector3::ZAxis, Vector3::XAxis, {1.0f, 1.0f}, botMove }); // 1 Bot Right
							waterMesh.vertices.push_back({ {maxX, topRightMaxY, posZ}, Vector3::ZAxis, Vector3::XAxis, {0.0f, 0.0f}, moveWater }); // 2 Top Left
							waterMesh.vertices.push_back({ {minX, topLeftMaxY, posZ}, Vector3::ZAxis, Vector3::XAxis, {1.0f, 0.0f}, moveWater }); // 3 Top Right

							const int count = static_cast<int>(waterMesh.vertices.size()) - (waterMesh.vertices.size() == 0 ? 0 : 4);

							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(3 + count);
							waterMesh.indices.push_back(1 + count);

							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(2 + count);
							waterMesh.indices.push_back(3 + count);

							addedBlock = true;
						}

						// South
						//if (false)
						if (IsBlockTransparent(data[x][y][z - 1].blockTypes) && data[x][y][z - 1].blockTypes != BlockTypes::Water) // && data[x][y][z - 1].padding < 7
						{
							const float posZ = z - size + offsetZ;
							const float minX = x - size + offsetX;
							const float maxX = x + size + offsetX;

							waterMesh.vertices.push_back({ {minX, minY, posZ}, -Vector3::ZAxis, Vector3::XAxis, {0.0f, 1.0f}, botMove }); // 0 Bot Left
							waterMesh.vertices.push_back({ {maxX, minY, posZ}, -Vector3::ZAxis, Vector3::XAxis, {1.0f, 1.0f}, botMove }); // 1 Bot Right
							waterMesh.vertices.push_back({ {minX, botLeftMaxY, posZ}, -Vector3::ZAxis, Vector3::XAxis, {0.0f, 0.0f}, moveWater }); // 2 Top Left
							waterMesh.vertices.push_back({ {maxX, botRightMaxY, posZ}, -Vector3::ZAxis, Vector3::XAxis, {1.0f, 0.0f}, moveWater }); // 3 Top Right

							const int count = static_cast<int>(waterMesh.vertices.size()) - (waterMesh.vertices.size() == 0 ? 0 : 4);

							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(3 + count);
							waterMesh.indices.push_back(1 + count);

							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(2 + count);
							waterMesh.indices.push_back(3 + count);

							addedBlock = true;
						}

						// East
						//if (false)
						if (IsBlockTransparent(data[x + 1][y][z].blockTypes) && data[x + 1][y][z].blockTypes != BlockTypes::Water) // && data[x + 1][y][z].padding < 7
						{
							const float posX = x + size + offsetX;
							const float minZ = z - size + offsetZ;
							const float maxZ = z + size + offsetZ;

							waterMesh.vertices.push_back({ {posX, minY, minZ}, Vector3::XAxis, Vector3::ZAxis, {0.0f, 1.0f}, botMove });	// 0 Bot Left
							waterMesh.vertices.push_back({ {posX, minY, maxZ}, Vector3::XAxis, Vector3::ZAxis, {1.0f, 1.0f}, botMove });	// 1 Bot Right 
							waterMesh.vertices.push_back({ {posX, botRightMaxY, minZ}, Vector3::XAxis, Vector3::ZAxis, {0.0f, 0.0f}, moveWater });	// 2 Top Left
							waterMesh.vertices.push_back({ {posX, topRightMaxY, maxZ}, Vector3::XAxis, Vector3::ZAxis, {1.0f, 0.0f}, moveWater });	// 3 Top Right

							const int count = static_cast<int>(waterMesh.vertices.size()) - (waterMesh.vertices.size() == 0 ? 0 : 4);

							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(3 + count);
							waterMesh.indices.push_back(1 + count);

							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(2 + count);
							waterMesh.indices.push_back(3 + count);

							addedBlock = true;
						}

						// West		
						//if (false)
						if (IsBlockTransparent(data[x - 1][y][z].blockTypes) && data[x - 1][y][z].blockTypes != BlockTypes::Water) // && data[x - 1][y][z].padding < 7
						{
							const float posX = x - size + offsetX;
							const float minZ = z - size + offsetZ;
							const float maxZ = z + size + offsetZ;

							waterMesh.vertices.push_back({ {posX, minY, maxZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, 1.0f}, botMove }); // 0 Bot Left
							waterMesh.vertices.push_back({ {posX, minY, minZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {1.0f, 1.0f}, botMove }); // 1 Bot Right 
							waterMesh.vertices.push_back({ {posX, topLeftMaxY, maxZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, 0.0f}, moveWater }); // 2 Top Left
							waterMesh.vertices.push_back({ {posX, botLeftMaxY, minZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {1.0f, 0.0f}, moveWater }); // 3 Top Right

							const int count = static_cast<int>(waterMesh.vertices.size()) - (waterMesh.vertices.size() == 0 ? 0 : 4);

							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(3 + count);
							waterMesh.indices.push_back(1 + count);

							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(2 + count);
							waterMesh.indices.push_back(3 + count);

							addedBlock = true;
						}

						//Top		
						//if (false)
						if ((y == 31 || (data[x][y + 1][z].blockTypes != BlockTypes::Water && IsBlockTransparent(data[x][y + 1][z].blockTypes)) ))
						{
							const float minX = x - size + offsetX;
							const float maxX = x + size + offsetX;
							const float minZ = z - size + offsetZ;
							const float maxZ = z + size + offsetZ;

							waterMesh.vertices.push_back({ {minX, botLeftMaxY, minZ}, Vector3::YAxis, Vector3::XAxis, {0.0f, 1.0f}, 1 }); // 0 Bot Left
							waterMesh.vertices.push_back({ {maxX, botRightMaxY, minZ}, Vector3::YAxis, Vector3::XAxis, {1.0f, 1.0f}, 1 }); // 1 Bot Right
							waterMesh.vertices.push_back({ {minX, topLeftMaxY, maxZ}, Vector3::YAxis, Vector3::XAxis, {0.0f, 0.0f}, 1 }); // 2 Top Left
							waterMesh.vertices.push_back({ {maxX, topRightMaxY, maxZ}, Vector3::YAxis, Vector3::XAxis, {1.0f, 0.0f}, 1 }); // 3 Top Right

							// Get indice count
							const int count = static_cast<int>(waterMesh.vertices.size()) - (waterMesh.vertices.size() == 0 ? 0 : 4);

							// Clockwise
							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(3 + count);
							waterMesh.indices.push_back(1 + count);

							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(2 + count);
							waterMesh.indices.push_back(3 + count);

							// Counter Clockwise
							waterMesh.indices.push_back(3 + count);
							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(1 + count);
							
							waterMesh.indices.push_back(2 + count);
							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(3 + count);

							addedBlock = true;
						}

						//Bot	
						//if (false)
						if (y < 31 && y > 0 && IsBlockTransparent(data[x][y - 1][z].blockTypes) && data[x][y - 1][z].blockTypes != BlockTypes::Water)
						{
							const float minX = x - size + offsetX;
							const float maxX = x + size + offsetX;
							const float minZ = z - size + offsetZ;
							const float maxZ = z + size + offsetZ;

							waterMesh.vertices.push_back({ {minX, minY, minZ}, -Vector3::YAxis, Vector3::XAxis, {0.0f, 1.0f}, 0 });	// 2 Top Left
							waterMesh.vertices.push_back({ {minX, minY, maxZ}, -Vector3::YAxis, Vector3::XAxis, {1.0f, 1.0f}, 0 });	// 3 Top Right
							waterMesh.vertices.push_back({ {maxX, minY, minZ}, -Vector3::YAxis, Vector3::XAxis, {0.0f, 0.0f}, 0 });	// 1 Bot Right
							waterMesh.vertices.push_back({ {maxX, minY, maxZ}, -Vector3::YAxis, Vector3::XAxis, {1.0f, 0.0f}, 0 });	// 0 Bot Left

							const int count = static_cast<int>(waterMesh.vertices.size()) - (waterMesh.vertices.size() == 0 ? 0 : 4);

							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(3 + count);
							waterMesh.indices.push_back(1 + count);

							waterMesh.indices.push_back(0 + count);
							waterMesh.indices.push_back(2 + count);
							waterMesh.indices.push_back(3 + count);
							addedBlock = true;
						}

						if (addedBlock)
						{
							chunkRenderer.activeBlocks.push_back({ x + offsetX, y + size, z + offsetZ });
							chunkRenderer.activeBlocksIndexs.push_back({ x - 1, y, z - 1 });
						}
					}
					else // Glass and leaves TODO: GreedyMesh
					{
						const auto blockData = mBlockData.at(data[x][y][z].blockTypes);

						//Front
						if (IsBlockTransparent(data[x][y][z + 1].blockTypes) || IsSlab(data[x][y][z + 1].blockTypes))
						{
							const float minX = x - size + offsetX;
							const float maxX = x + size + offsetX;
							const float minY = static_cast<float>(y);
							const float maxY = y + size + size;
							const float maxZ = z + size + offsetZ;

							transMesh.vertices.push_back({ {maxX, minY, maxZ}, Vector3::ZAxis, Vector3::XAxis, {0.0f, 1.0f}, blockData.northSideBlockIndex });	// 0 Bot Left
							transMesh.vertices.push_back({ {minX, minY, maxZ}, Vector3::ZAxis, Vector3::XAxis, {1.0f, 1.0f}, blockData.northSideBlockIndex });  // 1 Bot Right
							transMesh.vertices.push_back({ {maxX, maxY, maxZ}, Vector3::ZAxis, Vector3::XAxis, {0.0f, 0.0f}, blockData.northSideBlockIndex });	// 2 Top Left
							transMesh.vertices.push_back({ {minX, maxY, maxZ}, Vector3::ZAxis, Vector3::XAxis, {1.0f, 0.0f}, blockData.northSideBlockIndex });	// 3 Top Right

							const int count = static_cast<int>(transMesh.vertices.size()) - (transMesh.vertices.size() == 0 ? 0 : 4);

							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(3 + count);
							transMesh.indices.push_back(1 + count);

							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(2 + count);
							transMesh.indices.push_back(3 + count);

							addedBlock = true;
						}

						//Back
						if (IsBlockTransparent(data[x][y][z - 1].blockTypes) || IsSlab(data[x][y][z - 1].blockTypes))
						{
							const float minX = x - size + offsetX;
							const float maxX = x + size + offsetX;
							const float minY = static_cast<float>(y);
							const float maxY = y + size + size;
							const float minZ = z - size + offsetZ;

							transMesh.vertices.push_back({ {minX, minY, minZ}, -Vector3::ZAxis, Vector3::XAxis, {0.0f, 1.0f}, blockData.southSideBlockIndex });	// 0 Bot Left
							transMesh.vertices.push_back({ {maxX, minY, minZ}, -Vector3::ZAxis, Vector3::XAxis, {1.0f, 1.0f}, blockData.southSideBlockIndex });	// 1 Bot Right
							transMesh.vertices.push_back({ {minX, maxY, minZ}, -Vector3::ZAxis, Vector3::XAxis, {0.0f, 0.0f}, blockData.southSideBlockIndex });	// 2 Top Left
							transMesh.vertices.push_back({ {maxX, maxY, minZ}, -Vector3::ZAxis, Vector3::XAxis, {1.0f, 0.0f}, blockData.southSideBlockIndex });	// 3 Top Right

							const int count = static_cast<int>(transMesh.vertices.size()) - (transMesh.vertices.size() == 0 ? 0 : 4);

							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(3 + count);
							transMesh.indices.push_back(1 + count);

							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(2 + count);
							transMesh.indices.push_back(3 + count);

							addedBlock = true; 
						}

						//Left
						if (IsBlockTransparent(data[x + 1][y][z].blockTypes) || IsSlab(data[x + 1][y][z].blockTypes))
						{
							const float maxX = x + size + offsetX;
							const float minY = static_cast<float>(y);
							const float maxY = y + size + size;
							const float minZ = z - size + offsetZ;
							const float maxZ = z + size + offsetZ;

							transMesh.vertices.push_back({ {maxX, minY, minZ}, Vector3::XAxis, Vector3::ZAxis, {0.0f, 1.0f}, blockData.eastSideBlockIndex });	// 0 Bot Left
							transMesh.vertices.push_back({ {maxX, minY, maxZ}, Vector3::XAxis, Vector3::ZAxis, {1.0f, 1.0f}, blockData.eastSideBlockIndex });	// 1 Bot Right
							transMesh.vertices.push_back({ {maxX, maxY, minZ}, Vector3::XAxis, Vector3::ZAxis, {0.0f, 0.0f}, blockData.eastSideBlockIndex });	// 2 Top Left
							transMesh.vertices.push_back({ {maxX, maxY, maxZ}, Vector3::XAxis, Vector3::ZAxis, {1.0f, 0.0f}, blockData.eastSideBlockIndex });	// 3 Top Right

							const int count = static_cast<int>(transMesh.vertices.size()) - (transMesh.vertices.size() == 0 ? 0 : 4);

							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(3 + count);
							transMesh.indices.push_back(1 + count);

							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(2 + count);
							transMesh.indices.push_back(3 + count);

							addedBlock = true; 
						}

						//Right		
						if (IsBlockTransparent(data[x - 1][y][z].blockTypes) || IsSlab(data[x - 1][y][z].blockTypes))// == BlockTypes::Air)
						{
							const float minX = x - size + offsetX;
							const float minY = static_cast<float>(y);
							const float maxY = y + size + size;
							const float minZ = z - size + offsetZ;
							const float maxZ = z + size + offsetZ;

							transMesh.vertices.push_back({ {minX, minY, maxZ}, -Vector3::XAxis, Vector3::ZAxis, {0.0f, 1.0f}, blockData.westSideBlockIndex });	// 0 Bot Left
							transMesh.vertices.push_back({ {minX, minY, minZ}, -Vector3::XAxis, Vector3::ZAxis, {1.0f, 1.0f}, blockData.westSideBlockIndex });	// 1 Bot Right
							transMesh.vertices.push_back({ {minX, maxY, maxZ}, -Vector3::XAxis, Vector3::ZAxis, {0.0f, 0.0f}, blockData.westSideBlockIndex });	// 2 Top Left
							transMesh.vertices.push_back({ {minX, maxY, minZ}, -Vector3::XAxis, Vector3::ZAxis, {1.0f, 0.0f}, blockData.westSideBlockIndex });	// 3 Top Right

							const int count = static_cast<int>(transMesh.vertices.size()) - (transMesh.vertices.size() == 0 ? 0 : 4);

							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(3 + count);
							transMesh.indices.push_back(1 + count);

							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(2 + count);
							transMesh.indices.push_back(3 + count);

							addedBlock = true; 
						}

						//Top		
						if (y == 31 || IsBlockTransparent(data[x][y + 1][z].blockTypes))// == BlockTypes::Air)
						{
							const float minX = x - size + offsetX;
							const float maxX = x + size + offsetX;
							const float maxY = y + size + size;
							const float minZ = z - size + offsetZ;
							const float maxZ = z + size + offsetZ;

							transMesh.vertices.push_back({ {maxX, maxY, maxZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {0.0f, 1.0f}, blockData.topBlockIndex });	// 0 Bot Left
							transMesh.vertices.push_back({ {minX, maxY, maxZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {1.0f, 1.0f}, blockData.topBlockIndex });	// 1 Bot Right
							transMesh.vertices.push_back({ {maxX, maxY, minZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {0.0f, 0.0f}, blockData.topBlockIndex });	// 2 Top Left
							transMesh.vertices.push_back({ {minX, maxY, minZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {1.0f, 0.0f}, blockData.topBlockIndex });	// 3 Top Right

							const int count = static_cast<int>(transMesh.vertices.size()) - (transMesh.vertices.size() == 0 ? 0 : 4);

							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(3 + count);
							transMesh.indices.push_back(1 + count);

							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(2 + count);
							transMesh.indices.push_back(3 + count);

							addedBlock = true; 
						}

						//Bot	
						if (y < 31 && y > 0 && (IsBlockTransparent(data[x][y - 1][z].blockTypes) || IsSlab(data[x][y - 1][z].blockTypes)))
						{
							const float minX = x - size + offsetX;
							const float maxX = x + size + offsetX;
							const float minY = static_cast<float>(y);
							const float minZ = z - size + offsetZ;
							const float maxZ = z + size + offsetZ;

							transMesh.vertices.push_back({ {maxX, minY, maxZ}, -Vector3::YAxis, Vector3::XAxis, {0.0f, 1.0f}, blockData.botBlockIndex });	// 0 Bot Left
							transMesh.vertices.push_back({ {minX, minY, maxZ}, -Vector3::YAxis, Vector3::XAxis, {1.0f, 1.0f}, blockData.botBlockIndex });	// 1 Bot Right
							transMesh.vertices.push_back({ {maxX, minY, minZ}, -Vector3::YAxis, Vector3::XAxis, {0.0f, 0.0f}, blockData.botBlockIndex });	// 2 Top Left
							transMesh.vertices.push_back({ {minX, minY, minZ}, -Vector3::YAxis, Vector3::XAxis, {1.0f, 0.0f}, blockData.botBlockIndex });	// 3 Top Right

							const int count = static_cast<int>(transMesh.vertices.size()) - (transMesh.vertices.size() == 0 ? 0 : 4);

							transMesh.indices.push_back(3 + count);
							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(1 + count);

							transMesh.indices.push_back(2 + count);
							transMesh.indices.push_back(0 + count);
							transMesh.indices.push_back(3 + count);

							addedBlock = true;
						}

						if (addedBlock)
						{
							chunkRenderer.activeBlocks.push_back({ x + offsetX, y + size, z + offsetZ });
							chunkRenderer.activeBlocksIndexs.push_back({ x - 1, y, z - 1 });
						}
					}
				}
			}
		}
	}

	// Greedy Mesh Top
	for (int y = 0; y < 32; ++y)
	{
		if (y <= 30 && isYEmpty[y + 1])
		{
			continue;
		}

		bool myArray[16][16] = { false }; // Keep track of what blocks we have already checked
		for (int z = 1; z < 17; ++z) {
			for (int x = 1; x < 17; ++x) {

				if (myArray[x - 1][z - 1] == true || (y < 31 && !IsBlockTransparent(data[x][y + 1][z].blockTypes))) {
					myArray[x - 1][z - 1] = true;
					continue;
				}

				// Get Block Type
				const BlockTypes blockType = data[x][y][z].blockTypes;
				bool isSlab = IsSlab(data[x][y][z].blockTypes);

				// Create Rect
				RectInt rect(x - 1, z - 1, x, z);

				// Go right until you hit another block type or the wall
				for (int i = x; i < 16; ++i) {
					// If next block is the same type
					if (data[i + 1][y][z].blockTypes == blockType) {
						// If the block is covered
						if (y <= 30 && !IsBlockTransparent(data[i + 1][y + 1][z].blockTypes)) {
							break;
						}
						else if (myArray[i][z - 1] == true)
						{
							break;
						}
						else { // Block is not covered
							rect.max.x += 1;
						}
					}
					else {
						break;
					}
				}

				// Then go up until you hit another block type or roof sweeping as you check
				bool hitRoof = false;
				while (hitRoof == false)
				{
					// Hit Roof
					if (rect.max.y >= 16)
					{
						rect.max.y = 16;
						hitRoof = true;
						break;
					}

					const int sweepWidth = rect.max.x - rect.min.x;
					for (int i = 0; i < sweepWidth; ++i)
					{
						// If next block is not the same type
						if (data[x + i][y][z + rect.max.y - rect.min.y].blockTypes != blockType)
						{
							hitRoof = true;
							break;
						}
						else if (y < 31 && !IsBlockTransparent(data[x + i][y + 1][z + rect.max.y - rect.min.y].blockTypes))
						{
							// If the next block is the same type but the block above it isnt transparent
							hitRoof = true;
							break;
						}
					}

					if (hitRoof == false)
					{
						rect.max.y += 1;
					}
				}

				// Mark all in myArray to true rect
				for (int i = rect.min.y; i < rect.max.y; ++i)
				{
					for (int j = rect.min.x; j < rect.max.x; ++j)
					{
						myArray[j][i] = true;
					}
				}

				if (!IsBlockTransparent(blockType))
				{
					const auto& blockData = mBlockData.at(blockType);
					const float xRectOffSet = static_cast<float>(rect.max.x - rect.min.x - 1);
					const float zRectOffSet = static_cast<float>(rect.max.y - rect.min.y - 1);

					const float minX = x - size + offsetX;
					const float maxX = x + size + offsetX + xRectOffSet;
					const float minZ = z - size + offsetZ;
					const float maxZ = z + size + offsetZ + zRectOffSet;

					if (IsSlab(blockType))
					{
						const float posY = y + size;

						opaqueMesh.vertices.push_back({ {maxX, posY, maxZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, zRectOffSet + 1.0f}, blockData.topBlockIndex });
						opaqueMesh.vertices.push_back({ {maxX, posY, minZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, 0.0f}, blockData.topBlockIndex });
						opaqueMesh.vertices.push_back({ {minX, posY, minZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {0.0f, 0.0f}, blockData.topBlockIndex });
						opaqueMesh.vertices.push_back({ {minX, posY, maxZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {0.0f, zRectOffSet + 1.0f}, blockData.topBlockIndex });
					}
					else
					{
						const float posY = y + size + size;

						opaqueMesh.vertices.push_back({ {maxX, posY, maxZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, zRectOffSet + 1.0f}, blockData.topBlockIndex });
						opaqueMesh.vertices.push_back({ {maxX, posY, minZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, 0.0f}, blockData.topBlockIndex });
						opaqueMesh.vertices.push_back({ {minX, posY, minZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {0.0f, 0.0f}, blockData.topBlockIndex });
						opaqueMesh.vertices.push_back({ {minX, posY, maxZ}, Math::Vector3::YAxis, Math::Vector3::XAxis, {0.0f, zRectOffSet + 1.0f}, blockData.topBlockIndex });
					}

					const int count = static_cast<int>(opaqueMesh.vertices.size()) - (opaqueMesh.vertices.size() == 0 ? 0 : 4);

					opaqueMesh.indices.push_back(0 + count);
					opaqueMesh.indices.push_back(1 + count);
					opaqueMesh.indices.push_back(2 + count);

					opaqueMesh.indices.push_back(3 + count);
					opaqueMesh.indices.push_back(0 + count);
					opaqueMesh.indices.push_back(2 + count);
				}

				SkipCheckedBlocks(rect, x, z);
			}
		}
	}

	// Greedy Mesh Back
	for (int z = 1; z < 17; ++z) {
		bool myArray[16][32] = { false }; // Keep track of what blocks we have already checked
		for (int y = 0; y < 32; ++y) {
			for (int x = 1; x < 17; ++x) {

				if (myArray[x - 1][y] == true || !IsBlockTransparent(data[x][y][z - 1].blockTypes)) {
					myArray[x - 1][y] = true;
					continue;
				}

				// Get Block Type
				const BlockTypes blockType = data[x][y][z].blockTypes;

				// Create Rect
				RectInt rect(x - 1, y, x, y + 1);

				// Go right until you hit another block type or the wall
				for (int i = x; i < 16; ++i) {
					// If next block is the same type
					if (data[i + 1][y][z].blockTypes == blockType) {
						// If the block is covered
						if (!IsBlockTransparent(data[i + 1][y][z - 1].blockTypes)) {
							break;
						}
						else if (myArray[i][y] == true) {
							break;
						}
						else {
							rect.max.x += 1;
						}
					}
					else {
						break;
					}
				}

				// Then go up until you hit another block type or roof sweeping as you check
				bool hitRoof = false;
				while (hitRoof == false)
				{
					// Hit Roof
					if (rect.max.y >= 32)
					{
						rect.max.y = 32;
						hitRoof = true;
						break;
					}

					const int sweepWidth = rect.max.x - rect.min.x;
					for (int i = 0; i < sweepWidth; ++i)
					{
						// If next block is not the same type
						if (data[x + i][y + rect.max.y - rect.min.y][z].blockTypes != blockType)
						{
							hitRoof = true;
							break;
						}
						else if (!IsBlockTransparent(data[x + i][y + rect.max.y - rect.min.y][z - 1].blockTypes))
						{
							// If the next block is the same type but the block above it isnt transparent
							hitRoof = true;
							break;
						}
					}

					if (hitRoof == false)
					{
						rect.max.y += 1;
					}
				}

				// Mark all in myArray to true rect
				for (int i = rect.min.y; i < rect.max.y; ++i)
				{
					for (int j = rect.min.x; j < rect.max.x; ++j)
					{
						myArray[j][i] = true;
					}
				}

				if (!IsBlockTransparent(blockType)) {
					const auto& blockData = mBlockData.at(blockType);
					const float xRectOffSet = static_cast<float>(rect.max.x - rect.min.x - 1);
					const float yRectOffSet = static_cast<float>(rect.max.y - rect.min.y - 1);

					const float minX = x - size + offsetX;
					const float maxX = x + size + offsetX + xRectOffSet;
					const float minY = static_cast<float>(y);
					const float posZ = z - size + offsetZ;

					if (IsSlab(blockType))
					{
						const float maxY = y + size + yRectOffSet;

						opaqueMesh.vertices.push_back({ {minX, minY, posZ}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.0f, yRectOffSet + 1.0f}, blockData.southSideBlockIndex });
						opaqueMesh.vertices.push_back({ {minX, maxY, posZ}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.0f, 0.5f}, blockData.southSideBlockIndex });
						opaqueMesh.vertices.push_back({ {maxX, maxY, posZ}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, 0.5f}, blockData.southSideBlockIndex });
						opaqueMesh.vertices.push_back({ {maxX, minY, posZ}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, yRectOffSet + 1.0f}, blockData.southSideBlockIndex });
					}
					else
					{
						const float maxY = y + size + size + yRectOffSet;

						opaqueMesh.vertices.push_back({ {minX, minY, posZ}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.0f, yRectOffSet + 1.0f}, blockData.southSideBlockIndex });
						opaqueMesh.vertices.push_back({ {minX, maxY, posZ}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.0f, 0.0f}, blockData.southSideBlockIndex });
						opaqueMesh.vertices.push_back({ {maxX, maxY, posZ}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, 0.0f}, blockData.southSideBlockIndex });
						opaqueMesh.vertices.push_back({ {maxX, minY, posZ}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, yRectOffSet + 1.0f}, blockData.southSideBlockIndex });
					}

					const int count = static_cast<int>(opaqueMesh.vertices.size()) - (opaqueMesh.vertices.size() == 0 ? 0 : 4);

					opaqueMesh.indices.push_back(0 + count);
					opaqueMesh.indices.push_back(1 + count);
					opaqueMesh.indices.push_back(2 + count);

					opaqueMesh.indices.push_back(3 + count);
					opaqueMesh.indices.push_back(0 + count);
					opaqueMesh.indices.push_back(2 + count);
				}
				SkipCheckedBlocks(rect, x, y);
			}
		}
	}

	// Greedy Mesh Front
	for (int z = 1; z < 17; ++z) {
		bool myArray[16][32] = { false }; // Keep track of what blocks we have already checked
		for (int y = 0; y < 32; ++y) {
			for (int x = 1; x < 17; ++x) {

				if (myArray[x - 1][y] == true || !IsBlockTransparent(data[x][y][z + 1].blockTypes)) {
					myArray[x - 1][y] = true;
					continue;
				}

				// Get Block Type
				const BlockTypes blockType = data[x][y][z].blockTypes;

				// Create Rect
				RectInt rect(x - 1, y, x, y + 1);

				// Go right until you hit another block type or the wall
				for (int i = x; i < 16; ++i) {
					// If next block is the same type
					if (data[i + 1][y][z].blockTypes == blockType) {
						// If the block is covered
						if (!IsBlockTransparent(data[i + 1][y][z + 1].blockTypes)) {
							break;
						}
						else if (myArray[i][y] == true) {
							break;
						}
						else {
							rect.max.x += 1;
						}
					}
					else {
						break;
					}
				}

				// Then go up until you hit another block type or roof sweeping as you check
				bool hitRoof = false;
				while (hitRoof == false)
				{
					// Hit Roof
					if (rect.max.y >= 32)
					{
						rect.max.y = 32;
						hitRoof = true;
						break;
					}

					const int sweepWidth = rect.max.x - rect.min.x;
					for (int i = 0; i < sweepWidth; ++i)
					{
						// If next block is not the same type
						if (data[x + i][y + rect.max.y - rect.min.y][z].blockTypes != blockType)
						{
							hitRoof = true;
							break;
						}
						else if (!IsBlockTransparent(data[x + i][y + rect.max.y - rect.min.y][z + 1].blockTypes))
						{
							// If the next block is the same type but the block above it isnt transparent
							hitRoof = true;
							break;
						}
					}

					if (hitRoof == false)
					{
						rect.max.y += 1;
					}
				}

				// Mark all in myArray to true rect
				for (int i = rect.min.y; i < rect.max.y; ++i)
				{
					for (int j = rect.min.x; j < rect.max.x; ++j)
					{
						myArray[j][i] = true;
					}
				}

				if (!IsBlockTransparent(blockType)) {
					const auto& blockData = mBlockData.at(blockType);
					const float xRectOffSet = static_cast<float>(rect.max.x - rect.min.x - 1);
					const float yRectOffSet = static_cast<float>(rect.max.y - rect.min.y - 1);

					const float minX = x - size + offsetX;
					const float maxX = x + size + offsetX + xRectOffSet;
					const float minY = static_cast<float>(y);
					const float posZ = z + size + offsetZ;

					if (IsSlab(data[x][y][z].blockTypes))
					{
						const float maxY = y + size + yRectOffSet;

						opaqueMesh.vertices.push_back({ {minX, minY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.0f, yRectOffSet + 1.0f}, blockData.northSideBlockIndex });
						opaqueMesh.vertices.push_back({ {minX, maxY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.0f, 0.5f}, blockData.northSideBlockIndex });
						opaqueMesh.vertices.push_back({ {maxX, maxY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, 0.5f}, blockData.northSideBlockIndex });
						opaqueMesh.vertices.push_back({ {maxX, minY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, yRectOffSet + 1.0f}, blockData.northSideBlockIndex });
					}
					else
					{
						const float maxY = y + size + size + yRectOffSet;

						//opaqueMesh.vertices.push_back({ {minX, minY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, blockData.sideBotRightUV });
						//opaqueMesh.vertices.push_back({ {minX, maxY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, blockData.sideTopRightUV });
						//opaqueMesh.vertices.push_back({ {maxX, maxY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, blockData.sideBotLeftUV });
						//opaqueMesh.vertices.push_back({ {maxX, minY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, blockData.sideTopLeftUV });

						opaqueMesh.vertices.push_back({ {minX, minY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.0f, yRectOffSet + 1.0f}, blockData.northSideBlockIndex });
						opaqueMesh.vertices.push_back({ {minX, maxY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.0f, 0.0f}, blockData.northSideBlockIndex });
						opaqueMesh.vertices.push_back({ {maxX, maxY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, 0.0f}, blockData.northSideBlockIndex });
						opaqueMesh.vertices.push_back({ {maxX, minY, posZ}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, yRectOffSet + 1.0f}, blockData.northSideBlockIndex });
					}

					const int count = static_cast<int>(opaqueMesh.vertices.size()) - (opaqueMesh.vertices.size() == 0 ? 0 : 4);

					opaqueMesh.indices.push_back(2 + count);
					opaqueMesh.indices.push_back(1 + count);
					opaqueMesh.indices.push_back(0 + count);

					opaqueMesh.indices.push_back(2 + count);
					opaqueMesh.indices.push_back(0 + count);
					opaqueMesh.indices.push_back(3 + count);
				}

				SkipCheckedBlocks(rect, x, y);
			}
		}
	}

	// Greedy Mesh right
	for (int x = 1; x < 17; ++x) {
		bool myArray[16][32] = { false }; // Keep track of what blocks we have already checked
		for (int y = 0; y < 32; ++y) {
			for (int z = 1; z < 17; ++z) {

				if (myArray[z - 1][y] == true || !IsBlockTransparent(data[x - 1][y][z].blockTypes)) {
					myArray[z - 1][y] = true;
					continue;
				}

				// Get Block Type
				const BlockTypes blockType = data[x][y][z].blockTypes;

				// Create Rect
				RectInt rect(z - 1, y, z, y + 1);

				// Go right until you hit another block type or the wall
				for (int i = z; i < 16; ++i) {
					// If next block is the same type
					if (data[x][y][i + 1].blockTypes == blockType) {
						// If the block is covered
						if (!IsBlockTransparent(data[x - 1][y][i + 1].blockTypes)) {
							break;
						}
						else if (myArray[i][y] == true) {
							break;
						}
						else {
							rect.max.x += 1;
						}
					}
					else {
						break;
					}
				}

				// Then go up until you hit another block type or roof sweeping as you check
				bool hitRoof = false;
				while (hitRoof == false)
				{
					// Hit Roof
					if (rect.max.y >= 32)
					{
						rect.max.y = 32;
						hitRoof = true;
						break;
					}

					const int sweepWidth = rect.max.x - rect.min.x;
					for (int i = 0; i < sweepWidth; ++i)
					{
						// If next block is not the same type
						if (data[x][y + rect.max.y - rect.min.y][z + i].blockTypes != blockType)
						{
							hitRoof = true;
							break;
						}
						else if (!IsBlockTransparent(data[x - 1][y + rect.max.y - rect.min.y][z + i].blockTypes))
						{
							// If the next block is the same type but the block above it isnt transparent
							hitRoof = true;
							break;
						}
					}

					if (hitRoof == false)
					{
						rect.max.y += 1;
					}
				}

				// Mark all in myArray to true rect
				for (int i = rect.min.y; i < rect.max.y; ++i)
				{
					for (int j = rect.min.x; j < rect.max.x; ++j)
					{
						myArray[j][i] = true;
					}
				}

				if (!IsBlockTransparent(blockType)) {
					const auto& blockData = mBlockData.at(blockType);
					const float zRectOffSet = static_cast<float>(rect.max.x - rect.min.x - 1);
					const float yRectOffSet = static_cast<float>(rect.max.y - rect.min.y - 1);

					const float posX = x - size + offsetX;
					const float minY = static_cast<float>(y);
					const float minZ = z - size + offsetZ;
					const float maxZ = z + size + offsetZ + zRectOffSet;

					if (IsSlab(data[x][y][z].blockTypes))
					{
						const float maxY = y + size + yRectOffSet;

						opaqueMesh.vertices.push_back({ {posX, minY, minZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {zRectOffSet + 1.0f, yRectOffSet + 1.0f}, blockData.eastSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, maxY, minZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {zRectOffSet + 1.0f, 0.5f}, blockData.eastSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, maxY, maxZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, 0.5f}, blockData.eastSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, minY, maxZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, yRectOffSet + 1.0f}, blockData.eastSideBlockIndex });
					}
					else
					{
						const float maxY = y + size + size + yRectOffSet;

						//opaqueMesh.vertices.push_back({ {posX, minY, minZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, blockData.sideBotRightUV });
						//opaqueMesh.vertices.push_back({ {posX, maxY, minZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, blockData.sideTopRightUV });
						//opaqueMesh.vertices.push_back({ {posX, maxY, maxZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, blockData.sideBotLeftUV });
						//opaqueMesh.vertices.push_back({ {posX, minY, maxZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, blockData.sideTopLeftUV });

						opaqueMesh.vertices.push_back({ {posX, minY, minZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {zRectOffSet + 1.0f, yRectOffSet + 1.0f}, blockData.eastSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, maxY, minZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {zRectOffSet + 1.0f, 0.0f}, blockData.eastSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, maxY, maxZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, 0.0f}, blockData.eastSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, minY, maxZ}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, yRectOffSet + 1.0f}, blockData.eastSideBlockIndex });
					}

					const int count = static_cast<int>(opaqueMesh.vertices.size()) - (opaqueMesh.vertices.size() == 0 ? 0 : 4);

					opaqueMesh.indices.push_back(2 + count);
					opaqueMesh.indices.push_back(1 + count);
					opaqueMesh.indices.push_back(0 + count);

					opaqueMesh.indices.push_back(2 + count);
					opaqueMesh.indices.push_back(0 + count);
					opaqueMesh.indices.push_back(3 + count);
				}

				SkipCheckedBlocks(rect, z, y);
			}
		}
	}

	// Greedy Mesh left
	for (int x = 1; x < 17; ++x) {
		bool myArray[16][32] = { false }; // Keep track of what blocks we have already checked
		for (int y = 0; y < 32; ++y) {
			for (int z = 1; z < 17; ++z) {

				if (myArray[z - 1][y] == true || !IsBlockTransparent(data[x + 1][y][z].blockTypes)) {
					myArray[z - 1][y] = true;
					continue;
				}

				// Get Block Type
				const BlockTypes blockType = data[x][y][z].blockTypes;

				// Create Rect
				RectInt rect(z - 1, y, z, y + 1);

				// Go right until you hit another block type or the wall
				for (int i = z; i < 16; ++i) {
					// If next block is the same type
					if (data[x][y][i + 1].blockTypes == blockType) {
						// If the block is covered
						if (!IsBlockTransparent(data[x + 1][y][i + 1].blockTypes)) {
							break;
						}
						else if (myArray[i][y] == true) {
							break;
						}
						else {
							rect.max.x += 1;
						}
					}
					else {
						break;
					}
				}

				// Then go up until you hit another block type or roof sweeping as you check
				bool hitRoof = false;
				while (hitRoof == false)
				{
					// Hit Roof
					if (rect.max.y >= 32)
					{
						rect.max.y = 32;
						hitRoof = true;
						break;
					}

					const int sweepWidth = rect.max.x - rect.min.x;
					for (int i = 0; i < sweepWidth; ++i)
					{
						// If next block is not the same type
						if (data[x][y + rect.max.y - rect.min.y][z + i].blockTypes != blockType)
						{
							hitRoof = true;
							break;
						}
						else if (!IsBlockTransparent(data[x + 1][y + rect.max.y - rect.min.y][z + i].blockTypes))
						{
							// If the next block is the same type but the block above it isnt transparent
							hitRoof = true;
							break;
						}
					}

					if (hitRoof == false)
					{
						rect.max.y += 1;
					}
				}

				// Mark all in myArray to true rect
				for (int i = rect.min.y; i < rect.max.y; ++i)
				{
					for (int j = rect.min.x; j < rect.max.x; ++j)
					{
						myArray[j][i] = true;
					}
				}

				if (!IsBlockTransparent(blockType)) {
					const auto& blockData = mBlockData.at(blockType);
					const float zRectOffSet = static_cast<float>(rect.max.x - rect.min.x - 1);
					const float yRectOffSet = static_cast<float>(rect.max.y - rect.min.y - 1);

					const float posX = x + size + offsetX;
					const float minY = static_cast<float>(y);
					const float minZ = z - size + offsetZ;
					const float maxZ = z + size + offsetZ + zRectOffSet;

					if (IsSlab(data[x][y][z].blockTypes))
					{
						const float maxY = y + size + yRectOffSet;

						opaqueMesh.vertices.push_back({ {posX, minY, minZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {zRectOffSet + 1.0f, yRectOffSet + 1.0f}, blockData.westSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, maxY, minZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {zRectOffSet + 1.0f, 0.5f}, blockData.westSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, maxY, maxZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, 0.5f}, blockData.westSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, minY, maxZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, yRectOffSet + 1.0f}, blockData.westSideBlockIndex });
					}
					else
					{
						const float maxY = y + size + size + yRectOffSet;

						//opaqueMesh.vertices.push_back({ {posX, minY, minZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, blockData.sideBotRightUV });	//8  - LeftBotLeft
						//opaqueMesh.vertices.push_back({ {posX, maxY, minZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, blockData.sideTopRightUV });	//9  - LeftTopLeft
						//opaqueMesh.vertices.push_back({ {posX, maxY, maxZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, blockData.sideBotLeftUV });	//10 - LeftTopRight
						//opaqueMesh.vertices.push_back({ {posX, minY, maxZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, blockData.sideTopLeftUV });	//11 - LeftBotRight

						opaqueMesh.vertices.push_back({ {posX, minY, minZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {zRectOffSet + 1.0f, yRectOffSet + 1.0f}, blockData.westSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, maxY, minZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {zRectOffSet + 1.0f, 0.0f}, blockData.westSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, maxY, maxZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, 0.0f}, blockData.westSideBlockIndex });
						opaqueMesh.vertices.push_back({ {posX, minY, maxZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, yRectOffSet + 1.0f}, blockData.westSideBlockIndex });

						//const int thing = 2;
						//opaqueMesh.vertices.push_back({ {posX, minY, minZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {zRectOffSet + 1.0f, yRectOffSet + 1.0f}, thing });
						//opaqueMesh.vertices.push_back({ {posX, maxY, minZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {zRectOffSet + 1.0f, 0.0f}, thing });
						//opaqueMesh.vertices.push_back({ {posX, maxY, maxZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, 0.0f}, thing });
						//opaqueMesh.vertices.push_back({ {posX, minY, maxZ}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.0f, yRectOffSet + 1.0f}, thing });
					}

					const int count = static_cast<int>(opaqueMesh.vertices.size()) - (opaqueMesh.vertices.size() == 0 ? 0 : 4);

					opaqueMesh.indices.push_back(0 + count);
					opaqueMesh.indices.push_back(1 + count);
					opaqueMesh.indices.push_back(2 + count);

					opaqueMesh.indices.push_back(3 + count);
					opaqueMesh.indices.push_back(0 + count);
					opaqueMesh.indices.push_back(2 + count);
				}

				SkipCheckedBlocks(rect, z, y);
			}
		}
	}

	// Greedy Mesh Bottom
	for (int y = 1; y < 32; ++y)
	{
		if (isYEmpty[y - 1])
		{
			continue;
		}

		bool myArray[16][16] = { false }; // Keep track of what blocks we have already checked
		for (int z = 1; z < 17; ++z) {
			for (int x = 1; x < 17; ++x) {

				if (myArray[x - 1][z - 1] == true || y == 0 || (y > 0 && !IsBlockTransparent(data[x][y - 1][z].blockTypes))) {
					myArray[x - 1][z - 1] = true;
					continue;
				}

				// Get Block Type
				const BlockTypes blockType = data[x][y][z].blockTypes;

				// Create Rect
				RectInt rect(x - 1, z - 1, x, z);

				// Go right until you hit another block type or the wall
				for (int i = x; i < 16; ++i) {
					// If next block is the same type
					if (data[i + 1][y][z].blockTypes == blockType) {
						// If the block is covered
						if (y > 0 && !IsBlockTransparent(data[i + 1][y - 1][z].blockTypes)) {
							break;
						}
						else if (myArray[i][z - 1] == true)
						{
							break;
						}
						else { // Block is not covered
							rect.max.x += 1;
						}
					}
					else {
						break;
					}
				}

				// Then go up until you hit another block type or roof sweeping as you check
				bool hitRoof = false;
				while (hitRoof == false)
				{
					// Hit Roof
					if (rect.max.y >= 16)
					{
						rect.max.y = 16;
						hitRoof = true;
						break;
					}

					const int sweepWidth = rect.max.x - rect.min.x;
					for (int i = 0; i < sweepWidth; ++i)
					{
						// If next block is not the same type
						if (data[x + i][y][z + rect.max.y - rect.min.y].blockTypes != blockType)
						{
							hitRoof = true;
							break;
						}
						else if (y > 0 && !IsBlockTransparent(data[x + i][y - 1][z + rect.max.y - rect.min.y].blockTypes))
						{
							// If the next block is the same type but the block above it isnt transparent
							hitRoof = true;
							break;
						}
					}

					if (hitRoof == false)
					{
						rect.max.y += 1;
					}
				}

				// Mark all in myArray to true rect
				for (int i = rect.min.y; i < rect.max.y; ++i)
				{
					for (int j = rect.min.x; j < rect.max.x; ++j)
					{
						myArray[j][i] = true;
					}
				}

				if (!IsBlockTransparent(blockType)) {
					const auto& blockData = mBlockData.at(blockType);
					const float xRectOffSet = static_cast<float>(rect.max.x - rect.min.x - 1);
					const float zRectOffSet = static_cast<float>(rect.max.y - rect.min.y - 1);

					const float minX = x - size + offsetX;
					const float maxX = x + size + offsetX + xRectOffSet;
					const float posY = static_cast<float>(y);
					const float minZ = z - size + offsetZ;
					const float maxZ = z + size + offsetZ + zRectOffSet;

					//opaqueMesh.vertices.push_back({ {maxX, posY, maxZ}, -Math::Vector3::YAxis, Math::Vector3::XAxis, blockData.botBotRightUV });
					//opaqueMesh.vertices.push_back({ {maxX, posY, minZ}, -Math::Vector3::YAxis, Math::Vector3::XAxis, blockData.botTopRightUV });
					//opaqueMesh.vertices.push_back({ {minX, posY, minZ}, -Math::Vector3::YAxis, Math::Vector3::XAxis, blockData.botBotLeftUV });
					//opaqueMesh.vertices.push_back({ {minX, posY, maxZ}, -Math::Vector3::YAxis, Math::Vector3::XAxis, blockData.botTopLeftUV });

					opaqueMesh.vertices.push_back({ {maxX, posY, maxZ}, -Math::Vector3::YAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, zRectOffSet + 1.0f}, blockData.botBlockIndex });
					opaqueMesh.vertices.push_back({ {maxX, posY, minZ}, -Math::Vector3::YAxis, Math::Vector3::XAxis, {xRectOffSet + 1.0f, 0.0f}, blockData.botBlockIndex });
					opaqueMesh.vertices.push_back({ {minX, posY, minZ}, -Math::Vector3::YAxis, Math::Vector3::XAxis, {0.0f, 0.0f}, blockData.botBlockIndex });
					opaqueMesh.vertices.push_back({ {minX, posY, maxZ}, -Math::Vector3::YAxis, Math::Vector3::XAxis, {0.0f, zRectOffSet + 1.0f}, blockData.botBlockIndex });

					const int count = static_cast<int>(opaqueMesh.vertices.size()) - (opaqueMesh.vertices.size() == 0 ? 0 : 4);

					opaqueMesh.indices.push_back(2 + count);
					opaqueMesh.indices.push_back(1 + count);
					opaqueMesh.indices.push_back(0 + count);

					opaqueMesh.indices.push_back(2 + count);
					opaqueMesh.indices.push_back(0 + count);
					opaqueMesh.indices.push_back(3 + count);
				}

				SkipCheckedBlocks(rect, x, z);
			}
		}
	}

	chunkRenderer.vertices.clear();
	chunkRenderer.vertices.reserve(opaqueMesh.indices.size() + transMesh.indices.size());

	if (!opaqueMesh.vertices.empty())
	{
		for (int i = 0; i < opaqueMesh.indices.size(); ++i) {
			chunkRenderer.vertices.push_back(opaqueMesh.vertices[opaqueMesh.indices[i]].position);
		}

		chunkRenderer.opaqueChunkRenderObject.meshBuffer.Initialize(std::move(opaqueMesh));
	}

	chunkRenderer.transChunkActive = !transMesh.vertices.empty();
	if (chunkRenderer.transChunkActive)
	{
		for (int i = 0; i < transMesh.indices.size(); ++i) {
			chunkRenderer.vertices.push_back(transMesh.vertices[transMesh.indices[i]].position);
		}

		chunkRenderer.transparentChunkRenderObject.meshBuffer.Initialize(std::move(transMesh));
	}

	chunkRenderer.waterChunkActive = !waterMesh.vertices.empty();
	if (chunkRenderer.waterChunkActive)
	{
		chunkRenderer.waterChunkRenderObject.meshBuffer.Initialize(std::move(waterMesh));
	}
}

void ChunkMeshCreator::SkipCheckedBlocks(const RectInt& rect, int& right, int& up)
{
	const int width = rect.max.x - rect.min.x - 1;
	right += width;

	if (width == 16)
	{
		up += rect.max.y - rect.min.y - 1;
	}
}