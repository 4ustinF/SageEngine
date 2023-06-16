#pragma once

#include "TypeIds.h"

#include "ChunkMeshCreator.h"
#include <thread>
#include <queue>

class WorldService final : public SAGE::Service
{
public:
	SET_TYPE_ID(ServiceId::World);

	const std::string KeyGenerator(const SAGE::Math::Vector2Int& coords);

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

	void UpdateChunk(const SAGE::Math::Vector2Int& chunkCoords);
	void UpdateAABBS();

	std::unordered_map<std::string, ChunkData> mChunkData;
	std::unordered_map<std::string, ChunkRenderer> mChunkRenderObjects;

	SAGE::Math::Vector2Int mOldWorldCoords;
	SAGE::Math::Vector2Int mNewWorldCoords;

	std::vector<SAGE::Math::AABB> mAABBs;

private:
	SAGE::Graphics::VoxelTerrainEffect mVoxelTerrainEffect;
	SAGE::Graphics::ShadowEffect mShadowEffect;
	SAGE::Graphics::VoxelWaterEffect mVoxelWaterEffect;

	const SAGE::CameraService* mCameraService = nullptr;

	// ----------------------------------------------------------------------------------------------------
	const SAGE::Math::Vector3Int mChunkSize = SAGE::Math::Vector3Int(16, 32, 16);

	void InitializeNoise();
	BiomeTypes GetBiomeType(SAGE::Math::Vector2Int chunkCoords);
	ChunkData CreateChunkData(SAGE::Math::Vector2Int chunkCoords);
	ChunkData CreateDesertChunkData(SAGE::Math::Vector2Int chunkCoords);
	ChunkData CreateForestChunkData(SAGE::Math::Vector2Int chunkCoords);
	ChunkData CreateMountainChunkData(SAGE::Math::Vector2Int chunkCoords);
	ChunkData CreateOceanChunkData(SAGE::Math::Vector2Int chunkCoords);
	ChunkData CreatePlainsChunkData(SAGE::Math::Vector2Int chunkCoords);
	ChunkData CreateSnowyTundraChunkData(SAGE::Math::Vector2Int chunkCoords);

	bool IsBiomeSuperior(BiomeTypes currBiome, BiomeTypes otherBiome);
	std::vector<BiomeTypes> GetChunkRuleSet(BiomeTypes biome, SAGE::Math::Vector2Int chunkCoords);
	void DoChunkRuleSet(BiomeTypes biomeType, ChunkData& chunkData, int heightGen, int surfaceNoise, int x, int z);

	void DesertRuleSet(ChunkData& chunkData, int heightGen, int x, int z);
	void ForestRuleSet(ChunkData& chunkData, int heightGen, int surfaceNoise, int x, int z);
	void MountainRuleSet(ChunkData& chunkData, int heightGen, SAGE::Math::Vector2 perlin, int x, int z);
	void OceanRuleSet(ChunkData& chunkData, int heightGen, int surfaceNoise, int x, int z);
	void PlainRuleSet(ChunkData& chunkData, int heightGen, int surfaceNoise, int x, int z);
	void SnowyTundraRuleSet(ChunkData& chunkData, int heightGen, int surfaceNoise, int x, int z);

	void CreateTree(ChunkData& chunkData, SAGE::Math::Vector3Int basePosition);
	void BlendToOcean(int& heightGen, SAGE::Math::Vector2Int blockPos, bool nextToOceanRight, bool nextToOceanLeft, bool nextToOceanUp, bool nextToOceanDown);
	void BlendToOceanAngle(int& heightGen, SAGE::Math::Vector2Int blockPos, bool upRight, bool downRight, bool upLeft, bool downLeft);

	void InitializeMesh(SAGE::Math::Vector2Int chunkCoords, ChunkData& chunkData);
	void ChunkLoadCheck();
	void AssignReloadWall(ChunkWall unloadchunkWall);
	void RefreshChunks();
	void UpdateRenderDistance();
	void UpdateFogAmount();

	// Chunk modifications
	class BlockModification
	{
	public:
		BlockModification(BlockInfo blockType, SAGE::Math::Vector3Int position)
			: blockInfo(blockType), position(position)
		{
		}

		BlockInfo blockInfo;
		SAGE::Math::Vector3Int position = SAGE::Math::Vector3Int::Zero;
	};

	class ChunkModification
	{
	public:
		~ChunkModification()
		{
			mBlockModifications.clear();
		}

		std::vector<BlockModification> mBlockModifications;
	};
	void AddChunkModification(SAGE::Math::Vector2Int chunkCoords, BlockInfo blockInfo, SAGE::Math::Vector3Int position);
	void ModifyChunk(SAGE::Math::Vector2Int chunkCoords);
	std::unordered_map<std::string, ChunkModification> mChunkModifications;

	// Tick
	void Tick(float deltaTime);
	void ChunkTickUpdate(SAGE::Math::Vector2Int chunkCoord);
	void DirtTick(ChunkData& chunkData, const SAGE::Math::Vector3Int& blockIndex, bool& changedMesh);
	void SandGravelTick(ChunkData& chunkData, const SAGE::Math::Vector3Int& blockIndex, bool& changedMesh, const BlockTypes blockType);
	void WaterTick(ChunkData& chunkData, const SAGE::Math::Vector3Int& blockIndex, bool& changedMesh);
	void SugarCaneTick(ChunkData& chunkData, const SAGE::Math::Vector3Int& blockIndex, bool& changedMesh);
	void OakLeavesTick(ChunkData& chunkData, const SAGE::Math::Vector3Int& blockIndex, bool& changedMesh);
	void CactusTick(ChunkData& chunkData, const SAGE::Math::Vector3Int& blockIndex, bool& changedMesh);
	void WheatTick(ChunkData& chunkData, const SAGE::Math::Vector3Int& blockIndex, bool& changedMesh);

	// Sound Ids
	SAGE::Graphics::SoundId mLeafCrunchID = SAGE::Graphics::SoundEffectManager::Get()->Load("LeafCrunch.wav");;

	int mRenderDistance = 4;
	int mNewRenderDistance;

	float mTickStep = 1.0f;
	float mTickTimer = 0.0f;

	// Noise
	SAGE::Math::Noise mNoise;
	SAGE::Math::Noise mSurfaceNoise;
	SAGE::Math::Noise mCoalNoise;
	SAGE::Math::Noise mTreeZoneNoise;
	SAGE::Math::Noise mTreeNoise;
	SAGE::Math::Noise mHumidityNoise;
	SAGE::Math::Noise mTemperatureNoise;
	SAGE::Math::Noise mBiomeBlendingNoise;
	ChunkMeshCreator mChunkMeshCreator;

	const int mHeightOffSet = 17;
	const float mHeightIntensity = 25.0f;
	const int mOceanHeight = 16;

	bool mUseFog = false;
	SAGE::Graphics::Color mFogColor = SAGE::Graphics::Color(0.501960814f, 0.501960814f, 0.501960814f, 0.0f);

	const float mTreeZoneScale = 3.0f;
	const float mTreeZoneThreshold = 0.6f;
	const float mTreePlacementScale = 1.45f; // 1.4884f;
	const float mTreePlacementThreshold = .9f;
	const int mMaxTreeHeight = 12;
	const int mMinTreeHeight = 5;

	void CreateChunkRenderObject(const std::string key);

	struct ChunkToReload
	{
		SAGE::Math::Vector2Int oldCoords = SAGE::Math::Vector2Int::Zero;
		SAGE::Math::Vector2Int newCoords = SAGE::Math::Vector2Int::Zero;
	};
	std::queue<ChunkToReload> mChunksToReload = std::queue<ChunkToReload>();
	void ReloadChunk(ChunkToReload chunkToReload);

	std::queue<btRigidBody*> mRigidBodiesToRemove;
	void RemoveOldChunkRigidBodies();

	// Thread
	std::thread reloadChunks;
	bool mWorkerThreadActive = true;

	// Coroutine
	SAGE::Coroutine::Enumerator LiftChunk(const std::string loadChunkKey);
	bool mLiftChunk = false;
};